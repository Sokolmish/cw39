#include "copy_propagator.hpp"
#include "cfg_cleaner.hpp"


CopyPropagator::CopyPropagator(std::shared_ptr<ControlFlowGraph> rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(*rawCfg)) {
    // TODO: check for SSA form

    changed = true;
    while (changed) {
        changed = false;
//        remlacementMap.clear();
        visitedBlocks.clear();

        for (auto const &[fId, func] : cfg->getFuncs()) {
            traverseBlocks(func.getEntryBlockId(), [this](int blockId) {
                auto &curBlock = cfg->block(blockId);

                for (auto *node : curBlock.getAllNodes()) {
                    if (node->res && node->body && node->body->type == IR_Expr::OPERATION) {
                        auto oper = dynamic_cast<IR_ExprOper &>(*node->body);
                        if (oper.op == IR_MOV) {
                            changed = true;
                            remlacementMap.emplace(*node->res, oper.args.at(0));
                            node->body = nullptr;
                        }
                    }

                    if (node->body) {
                        for (auto *arg: node->body->getArgs()) {
                            auto it = remlacementMap.find(*arg);
                            if (it != remlacementMap.end()) {
                                changed = true;
                                *arg = it->second;
                            }
                        }
                    }
                }
            });
        }
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

void CopyPropagator::traverseBlocks(int blockId, std::function<void(int)> action) {
    if (visitedBlocks.contains(blockId))
        return;
    action(blockId);
    for (int nextId : cfg->block(blockId).next)
        if (!visitedBlocks.contains(nextId))
            traverseBlocks(nextId, action);
}
