#include "copy_propagator.hpp"
#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include "ir/generator.hpp"
#include "ir/constants_folder.hpp"


CopyPropagator::CopyPropagator(CFGraph rawCfg) : IRTransformer(std::move(rawCfg)) {
    changed = true;
    while (changed) {
        changed = false;
        propagateCopies();
        foldConstants();
    }

    CfgCleaner cleaner(std::move(cfg));
    cleaner.removeNops();
    cleaner.removeUselessNodes();
    cleaner.removeUnreachableBlocks();
    cleaner.removeTransitBlocks();
    cfg = std::move(cleaner).moveCfg();
}

void CopyPropagator::propagateCopies() {
    std::set<int> visited;

    changed = true;
    while (changed) {
        changed = false;
        visited.clear();

        cfg.traverseBlocks(cfg.entryBlockId, visited, [this](int blockId) {
            auto &curBlock = cfg.block(blockId);

            for (IR_Node *node : curBlock.getAllNodes()) {
                if (!node->body)
                    continue;

                if (node->res && node->res->isVReg() && node->body->type == IR_Expr::OPERATION) {
                    auto oper = dynamic_cast<IR_ExprOper &>(*node->body);
                    if (oper.op == IR_ExprOper::MOV) {
                        changed = true;
                        setPassChanged();
                        remlacementMap.emplace(*node->res, oper.args.at(0));
                        *node = IR_Node::nop();
                    }
                }

                if (node->body) {
                    for (auto *arg : node->body->getArgs()) {
                        auto it = remlacementMap.find(*arg);
                        if (it != remlacementMap.end()) {
                            changed = true;
                            setPassChanged();
                            *arg = it->second;
                        }
                    }
                }
            }
        });
    }
}

void CopyPropagator::foldConstants() {
    std::set<int> visited;

    changed = true;
    while (changed) {
        changed = false;
        visited.clear();

        cfg.traverseBlocks(cfg.entryBlockId, visited, [this](int blockId) {
            auto &curBlock = cfg.block(blockId);
            for (auto *node : curBlock.getAllNodes()) {
                if (!node->body)
                    continue;

                // TODO: access operations
                if (node->body->type == IR_Expr::OPERATION) {
                    auto &operExpr = dynamic_cast<IR_ExprOper &>(*node->body);

                    if (operExpr.op == IR_ExprOper::MOV)
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

                    auto newVal = ConstantsFolder::foldExpr(operExpr);
                    if (newVal) {
                        changed = true;
                        setPassChanged();
                        operExpr.op = IR_ExprOper::MOV;
                        operExpr.args = { *newVal };
                    }
                }
                else if (node->body->type == IR_Expr::CAST) {
                    auto &castExpr = dynamic_cast<IR_ExprCast &>(*node->body);

                    if (!castExpr.arg.isConstant())
                        continue;

//                        changed = true;
//                        setPassChanged();
                    // TODO
                }
                else if (node->body->type == IR_Expr::PHI) {
                    auto &phiExpr = dynamic_cast<IR_ExprPhi &>(*node->body);

                    IRval commonVal = phiExpr.args.at(0);

                    bool isConst = true;
                    for (auto const &[pos, arg] : phiExpr.args) {
                        // TODO: also collapse on same non-const values
                        if (!arg.isConstant() || arg != commonVal) {
                            isConst = false;
                            break;
                        }
                    }
                    if (!isConst)
                        continue;

                    changed = true;
                    setPassChanged();
                    node->body = std::make_unique<IR_ExprOper>(
                            IR_ExprOper::MOV, std::vector<IRval>{ commonVal });
                }
            }
        });
    }
}
