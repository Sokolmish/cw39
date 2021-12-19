#include "cfg_cleaner.hpp"

CfgCleaner::CfgCleaner(std::shared_ptr<ControlFlowGraph> rawCfg)
        : cfg(std::make_shared<ControlFlowGraph>(*rawCfg)) {}

std::shared_ptr<ControlFlowGraph> CfgCleaner::getCfg() {
    return cfg;
}

void CfgCleaner::removeNops() {
    for (auto const &[bId, block] : cfg->getBlocks()) {
        std::vector<IR_Node> newBody;
        for (IR_Node &node : cfg->block(bId).body)
            if (node.body)
                newBody.push_back(std::move(node));
        cfg->block(bId).body = std::move(newBody);
    }
}

void CfgCleaner::fixVersions() {
    std::map<IRval, std::pair<bool, int>, IRval::Comparator> versionedRegs;

    visitedBlocks.clear();
    for (auto const &[fId, func]: cfg->getFuncs()) {
        if (visitedBlocks.contains(func.getEntryBlockId()))
            continue;

        traverseBlocks(func.getEntryBlockId(), [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg->block(blockId);

            for (auto const &phiNode: curBlock.phis) {
                if (phiNode.res) {
                    int curVers = *phiNode.res->version;
                    auto it = versionedRegs.lower_bound(*phiNode.res);
                    if (it != versionedRegs.end() && it->first == *phiNode.res) {
                        if (it->second.second != curVers)
                            it->second = std::make_pair(true, 0);
                    }
                    else {
                        versionedRegs.emplace_hint(it, *phiNode.res, std::make_pair(false, curVers));
                    }
                }
            }

            for (auto const &node : curBlock.body) {
                if (node.res) {
                    int curVers = *node.res->version;
                    auto it = versionedRegs.lower_bound(*node.res);
                    if (it != versionedRegs.end() && it->first == *node.res) {
                        if (it->second.second != curVers)
                            it->second = std::make_pair(true, 0);
                    }
                    else {
                        versionedRegs.emplace_hint(it, *node.res, std::make_pair(false, curVers));
                    }
                }
            }
        });
    }

    visitedBlocks.clear();
    for (auto const &[fId, func]: cfg->getFuncs()) {
        if (visitedBlocks.contains(func.getEntryBlockId()))
            continue;

        traverseBlocks(func.getEntryBlockId(), [this, &versionedRegs](int blockId) {
            auto &curBlock = cfg->block(blockId);

            for (auto &phiNode: curBlock.phis) {
                if (phiNode.res) {
                    auto it = versionedRegs.find(*phiNode.res);
                    if (it != versionedRegs.end() && !it->second.first)
                        phiNode.res->version = {};
                }
                for (auto *arg : phiNode.body->getArgs()) {
                    auto it = versionedRegs.find(*arg);
                    if (it != versionedRegs.end() && !it->second.first)
                        arg->version = {};
                }
            }

            for (auto &node: curBlock.body) {
                if (node.res) {
                    auto it = versionedRegs.find(*node.res);
                    if (it != versionedRegs.end() && !it->second.first)
                        node.res->version = {};
                }
                for (auto *arg : node.body->getArgs()) {
                    auto it = versionedRegs.find(*arg);
                    if (it != versionedRegs.end() && !it->second.first)
                        arg->version = {};
                }
            }

            if (curBlock.terminator.arg) {
                auto it = versionedRegs.find(*curBlock.terminator.arg);
                if (it != versionedRegs.end() && !it->second.first)
                    curBlock.terminator.arg->version = {};
            }
        });
    }
}


void CfgCleaner::traverseBlocks(int blockId, std::function<void(int)> action) {
    action(blockId);
    for (int nextId : cfg->block(blockId).next)
        if (!visitedBlocks.contains(nextId))
            traverseBlocks(nextId, action);
}
