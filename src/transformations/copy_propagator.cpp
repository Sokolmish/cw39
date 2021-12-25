#include "copy_propagator.hpp"
#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include "ir/generator.hpp"


CopyPropagator::CopyPropagator(std::shared_ptr<ControlFlowGraph> rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(*rawCfg)) {

    globalChanged = true;
    while (globalChanged) {
        globalChanged = false;
        propagateCopies();
        foldConstants();
    }

    CfgCleaner cleaner(cfg);
    cleaner.removeNops();
    cleaner.fixVersions();
    cleaner.removeUselessNodes();
    cfg = cleaner.getCfg();

    rawCfg.reset();
}

std::shared_ptr<ControlFlowGraph> CopyPropagator::getCfg() {
    return cfg;
}

void CopyPropagator::propagateCopies() {
    std::set<int> visited;

    changed = true;
    while (changed) {
        changed = false;
        visited.clear();

        for (auto const &[fId, func]: cfg->getFuncs()) {
            cfg->traverseBlocks(func.getEntryBlockId(), visited, [this](int blockId) {
                auto &curBlock = cfg->block(blockId);

                for (auto *node: curBlock.getAllNodes()) {
                    if (node->res && node->res->isVReg() && node->body && node->body->type == IR_Expr::OPERATION) {
                        auto oper = dynamic_cast<IR_ExprOper &>(*node->body);
                        if (oper.op == IR_MOV) {
                            changed = true;
                            globalChanged = true;
                            remlacementMap.emplace(*node->res, oper.args.at(0));
                            node->body = nullptr;
                        }
                    }

                    if (node->body) {
                        for (auto *arg: node->body->getArgs()) {
                            auto it = remlacementMap.find(*arg);
                            if (it != remlacementMap.end()) {
                                changed = true;
                                globalChanged = true;
                                *arg = it->second;
                            }
                        }
                    }
                }
            });
        }
    }
}

void CopyPropagator::foldConstants() {
    std::set<int> visited;

    changed = true;
    while (changed) {
        changed = false;
        visited.clear();

        for (auto const &[fId, func]: cfg->getFuncs()) {
            cfg->traverseBlocks(func.getEntryBlockId(), visited, [this](int blockId) {
                auto &curBlock = cfg->block(blockId);
                for (auto *node: curBlock.getAllNodes()) {
                    if (!node->body)
                        continue;

                    if (node->body->type == IR_Expr::OPERATION) {
                        auto &operExpr = dynamic_cast<IR_ExprOper &>(*node->body);

                        if (isInList(operExpr.op, { IR_MOV, IR_STORE, IR_LOAD, IR_INSERT, IR_EXTRACT }))
                            continue;

                        bool isConst = true;
                        for (auto const &arg : operExpr.args) {
                            if (!arg.isConstant()) {
                                isConst = false;
                                break;
                            }
                        }
                        if (!isConst)
                            continue;

                        changed = true;
                        globalChanged = true;
                        IRval newVal = doConstOperation(operExpr);
                        operExpr.op = IR_MOV;
                        operExpr.args = { newVal };
                    }
                    else if (node->body->type == IR_Expr::CAST) {
                        auto &castExpr = dynamic_cast<IR_ExprCast &>(*node->body);

                        if (!castExpr.arg.isConstant())
                            continue;

//                        changed = true;
//                        globalChanged = true;
                        // TODO
                    }
                    else if (node->body->type == IR_Expr::PHI) {
                        auto &phiExpr = dynamic_cast<IR_ExprPhi &>(*node->body);

                        IRval commonVal = phiExpr.args.at(0);

                        bool isConst = true;
                        for (auto const &arg : phiExpr.args) {
                            // TODO: check comparsion
                            if (!arg.second.isConstant() || !(arg.second == commonVal)) {
                                isConst = false;
                                break;
                            }
                        }
                        if (!isConst)
                            continue;

                        changed = true;
                        globalChanged = true;
                        node->body = std::make_unique<IR_ExprOper>(
                                IR_MOV, std::vector<IRval>{ commonVal });
                    }
                }
            });
        }
    }
}

IRval CopyPropagator::doConstOperation(const IR_ExprOper &oper) {
    using bop = AST_Binop;

    switch (oper.op) {
        case IR_MUL:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_DIV:
            return IR_Generator::doConstBinOperation(bop::DIV, oper.args[0], oper.args[1]);
        case IR_REM:
            return IR_Generator::doConstBinOperation(bop::REM, oper.args[0], oper.args[1]);
        case IR_ADD:
            return IR_Generator::doConstBinOperation(bop::ADD, oper.args[0], oper.args[1]);
        case IR_SUB:
            return IR_Generator::doConstBinOperation(bop::SUB, oper.args[0], oper.args[1]);
        case IR_SHR:
            return IR_Generator::doConstBinOperation(bop::SHR, oper.args[0], oper.args[1]);
        case IR_SHL:
            return IR_Generator::doConstBinOperation(bop::SHL, oper.args[0], oper.args[1]);
        case IR_XOR:
            return IR_Generator::doConstBinOperation(bop::BIT_XOR, oper.args[0], oper.args[1]);
        case IR_AND:
            return IR_Generator::doConstBinOperation(bop::BIT_AND, oper.args[0], oper.args[1]);
        case IR_OR:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_EQ:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_NE:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_GT:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_LT:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_GE:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);
        case IR_LE:
            return IR_Generator::doConstBinOperation(bop::MUL, oper.args[0], oper.args[1]);

        case IR_MOV:
        case IR_LOAD:
        case IR_STORE:
        case IR_EXTRACT:
        case IR_INSERT:
        case IR_LAND:
        case IR_LOR:
        default:
            semanticError("Unsupported operation");
    }
}
