#include "computing_transformers.hpp"
#include "cfg_cleaner.hpp"
#include "utils.hpp"
#include "ir/generator.hpp"
#include "ir/constants_folder.hpp"

CopyPropagator::CopyPropagator(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)) {
    changed = true;
    while (changed) {
        changed = false;
        propagateCopies();
        foldConstants();
        if (changed)
            setPassChanged();

        CfgCleaner cleaner(unit, std::move(cfg));
        cleaner.removeNops();
        cleaner.removeUselessNodes();
        cleaner.removeUnreachableBlocks();
        cleaner.removeTransitBlocks();
        if (cleaner.isPassEffective())
            setPassChanged();
        cfg = std::move(cleaner).moveCfg();
    }
}

void CopyPropagator::propagateCopies() {
    std::set<int> visited;

    bool localChanged = true;

    while (localChanged) {
        localChanged = false;
        visited.clear();

        cfg.traverseBlocks(cfg.entryBlockId, visited, [this, &localChanged](int blockId) {
            auto &curBlock = cfg.block(blockId);

            for (IR_Node *node : curBlock.getAllNodes()) {
                if (!node->body)
                    continue;

                if (node->res && node->res->isVReg()) {
                    if (auto oper = dynamic_cast<IR_ExprOper *>(node->body.get())) {
                        if (oper->op == IR_ExprOper::MOV) {
                            localChanged = true;
                            setPassChanged();
                            remlacementMap.emplace(*node->res, oper->args.at(0));
                            *node = IR_Node::nop();
                        }
                    }
                }

                if (node->body) {
                    for (auto *arg : node->body->getArgs()) {
                        auto it = remlacementMap.find(*arg);
                        if (it != remlacementMap.end()) {
                            localChanged = true;
                            setPassChanged();
                            *arg = it->second;
                        }
                    }
                }
            }
        });

        if (localChanged)
            changed = true;
    }
}

void CopyPropagator::foldConstants() {
    std::set<int> visited;
    bool localChanged = true;

    while (localChanged) {
        localChanged = false;
        visited.clear();

        cfg.traverseBlocks(cfg.entryBlockId, visited, [this, &localChanged](int blockId) {
            auto &curBlock = cfg.block(blockId);
            for (auto *node : curBlock.getAllNodes()) {
                if (!node->body)
                    continue;

                if (auto operExpr = dynamic_cast<IR_ExprOper *>(node->body.get())) {
                    if (operExpr->op == IR_ExprOper::MOV)
                        continue;

                    bool isConst = true;
                    for (auto const &arg : operExpr->args) {
                        if (!arg.isConstant()) {
                            isConst = false;
                            break;
                        }
                    }
                    if (!isConst)
                        continue;

                    auto newVal = ConstantsFolder::foldExpr(*operExpr);
                    if (newVal) {
                        localChanged = true;
                        setPassChanged();
                        operExpr->op = IR_ExprOper::MOV;
                        operExpr->args = { *newVal };
                    }
                }
                else if (auto castExpr = dynamic_cast<IR_ExprCast *>(node->body.get())) {
                    if (!castExpr->arg.isConstant())
                        continue;

//                        localChanged = true;
//                        setPassChanged();
                    // TODO
                }
                else if (auto phiExpr = dynamic_cast<IR_ExprPhi *>(node->body.get())) {
                    IRval commonVal = phiExpr->args.at(0);

                    bool isConst = true;
                    for (auto const &[pos, arg] : phiExpr->args) {
                        // TODO: also collapse on same non-const values
                        if (!arg.isConstant() || arg != commonVal) {
                            isConst = false;
                            break;
                        }
                    }
                    if (!isConst)
                        continue;

                    localChanged = true;
                    setPassChanged();
                    node->body = std::make_unique<IR_ExprOper>(
                            IR_ExprOper::MOV, std::vector<IRval>{ commonVal });
                }
            }
        });

        if (localChanged)
            changed = true;
    }
}
