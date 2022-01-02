#include "cfg_cleaner.hpp"
#include "utils.hpp"

CfgCleaner::CfgCleaner(std::shared_ptr<ControlFlowGraph> rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(*rawCfg)) {}

std::shared_ptr<ControlFlowGraph> CfgCleaner::getCfg() {
    return cfg;
}

void CfgCleaner::removeNops() {
    for (auto const &[bId, block] : cfg->getBlocks()) {
        std::vector<IR_Node> newPhis;
        for (IR_Node &node : cfg->block(bId).phis)
            if (node.body)
                newPhis.push_back(std::move(node));
        cfg->block(bId).phis = std::move(newPhis);

        std::vector<IR_Node> newBody;
        for (IR_Node &node : cfg->block(bId).body)
            if (node.body)
                newBody.push_back(std::move(node));
        cfg->block(bId).body = std::move(newBody);
    }
}

void CfgCleaner::fixVersions() {
    std::set<int> visited;
    std::map<IRval, std::pair<bool, int>, IRval::Comparator> versionedRegs;

    visited.clear();
    for (auto const &[fId, func]: cfg->getFuncs()) {
        cfg->traverseBlocks(func.getEntryBlockId(), visited, [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg->block(blockId);
            for (IR_Node *node: curBlock.getAllNodes()) {
                if (node->res) {
                    int curVers = *node->res->version;
                    auto it = versionedRegs.lower_bound(*node->res);
                    if (it != versionedRegs.end() && it->first == *node->res) {
                        if (it->second.second != curVers)
                            it->second = std::make_pair(true, 0);
                    }
                    else {
                        versionedRegs.emplace_hint(it, *node->res, std::make_pair(false, curVers));
                    }
                }
            }
        });
    }

    visited.clear();
    for (auto const &[fId, func]: cfg->getFuncs()) {
        cfg->traverseBlocks(func.getEntryBlockId(), visited, [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg->block(blockId);

            for (auto *node: curBlock.getAllNodes()) {
                if (node->res) {
                    auto it = versionedRegs.find(*node->res);
                    if (it != versionedRegs.end() && !it->second.first)
                        node->res->version = {};
                }
                for (IRval *arg : node->body->getArgs()) {
                    if (arg->version && arg->version < 0) {
                        *arg = IRval::createUndef(arg->getType());
                    }
                    else {
                        auto it = versionedRegs.find(*arg);
                        if (it != versionedRegs.end() && !it->second.first)
                            arg->version = {};
                    }
                }
            }
        });
    }
}

void CfgCleaner::removeUselessNodes() {
    std::set<int> visited;
    std::set<IRval, IRval::ComparatorVersions> usedRegs;
    bool changed = true;

    while (changed) {
        changed = false;
        usedRegs.clear();

        visited.clear();
        for (auto const &[fId, func]: cfg->getFuncs()) {
            cfg->traverseBlocks(func.getEntryBlockId(), visited, [this, &usedRegs](int blockId) {
                auto &curBlock = cfg->block(blockId);
                auto refs = curBlock.getReferences();
                usedRegs.insert(refs.begin(), refs.end());
            });
        }

        visited.clear();
        for (auto const &[fId, func]: cfg->getFuncs()) {
            cfg->traverseBlocks(func.getEntryBlockId(), visited, [this, &usedRegs, &changed](int blockId) {
                auto &curBlock = cfg->block(blockId);
                for (auto *node: curBlock.getAllNodes()) {
                    if (node->res && node->res->isVReg() && !usedRegs.contains(*node->res)) {
                        changed = true;
                        node->res = {};
                        if (node->body->type == IR_Expr::CALL)
                            continue;
                        else if (node->body->type == IR_Expr::OPERATION) {
                            if (isInList(node->body->getOper().op, { IR_ExprOper::STORE, IR_ExprOper::INSERT }))
                                continue;
                        }
                        node->body = nullptr;
                    }
                }
            });
        }
    }
}
