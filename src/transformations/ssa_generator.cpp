#include "ssa_generator.hpp"
#include "cfg_cleaner.hpp"

SSA_Generator::SSA_Generator(std::shared_ptr<ControlFlowGraph> in_cfg)
        : origCfg(in_cfg) {
    cfg = std::make_shared<ControlFlowGraph>(*origCfg);
    doms = std::make_unique<Dominators>(cfg);

    placePhis();
    versionize();

    CfgCleaner cleaner(cfg);
    cleaner.fixVersions();
    cleaner.removeUselessNodes();
    cleaner.removeNops();
    cfg = cleaner.getCfg();

    origCfg.reset();
}

std::shared_ptr<ControlFlowGraph> SSA_Generator::getCfg() {
    return cfg;
}

// PHIs placing

void SSA_Generator::placePhis() {
    int counter = 0;
    std::vector<bool> visited(cfg->getBlocks().size());
    for (auto const &[fId, func] : cfg->getFuncs()) {
        makePostOrder(visited, counter, func.getEntryBlockId());
    }

    makeVerticesDF();

    // For each variable find, where a value assigned to it
    std::map<IRval, std::set<int>, IRval::Comparator> varsDefSet;
    for (auto const &[bId, block] : cfg->getBlocks())
        for (IRval const &def : block.getDefinitions())
            varsDefSet[def].insert(bId);

    for (auto const &[var, varDefs] : varsDefSet) {
        std::set<int> JP = getSetDFP(varDefs); // JP == DFP (theorem)
        for (int blockId : JP) {
//            cfg->block(blockId).phis.insert({ var, {} });
            auto phiNode = IR_Node(var, std::make_unique<IR_ExprPhi>());
            cfg->block(blockId).phis.push_back(std::move(phiNode));
        }
    }
}

void SSA_Generator::makePostOrder(std::vector<bool> &visited, int &counter, int cur) {
    visited.at(cur) = true;
    for (int nextId : cfg->block(cur).next)
        if (!visited.at(nextId))
            makePostOrder(visited, counter, nextId);
    postOrder.insert({ cur, counter++ });
}

void SSA_Generator::makeVerticesDF() {
    // Sort blocks by post order
    std::vector<int> sortedIds;
    for (auto const &[bId, block] : cfg->getBlocks())
        if (postOrder.contains(bId)) // TODO: get set from makePostOrder
            sortedIds.push_back(bId);
    std::sort(sortedIds.begin(), sortedIds.end(), [this](int a, int b) {
        return postOrder.at(a) < postOrder.at(b);
    });

    for (int xId : sortedIds) {
        auto it = verticesDF.emplace(xId, std::set<int>());
        auto &curSet = it.first->second;
        for (int yId : cfg->block(xId).next) {
            if (doms->getIdom(yId) != xId) {
                curSet.insert(yId);
            }
        }
        for (int z : doms->getChildren(xId)) {
            for (int y : verticesDF.at(z)) {
                if (doms->getIdom(y) != xId) {
                    curSet.insert(y);
                }
            }
        }
    }
}

std::set<int> SSA_Generator::getSetDF(const std::set<int> &S) const {
    std::set<int> res;
    for (int x : S)
        res.insert(verticesDF.at(x).cbegin(), verticesDF.at(x).cend());
    return res;
}

std::set<int> SSA_Generator::getSetDFP(const std::set<int> &S) const {
    std::set<int> res;
    std::set<int> DFP;
    bool changed = true;
    while (changed) {
        changed = false;
        std::set<int> tmp = S;
        tmp.insert(DFP.cbegin(), DFP.cend());
        DFP = getSetDF(tmp);
        if (res != DFP) {
            res = DFP;
            changed = true;
        }
    }
    return res;
}


// Versionizing

void SSA_Generator::versionize() {
    // TODO: traverse blocks for functions
    // Collect variables from graph because its was not passed in CFG
    std::set<IRval, IRval::Comparator> variables;
    for (auto const &[bId, block] : cfg->getBlocks()) {
        for (IRval def : block.getDefinitions())
            variables.insert(def);
        for (IRval ref : block.getReferences())
            variables.insert(ref);
    }

//    versions = std::make_unique<std::deque<int>>();
    for (IRval var : variables) {
        versionsCnt = 0;
        versions.clear();
        versions.push_back(-1); // In case of uninitialized variable

        for (auto const &[fId, func] : cfg->getFuncs()) {
            traverseForVar(func.getEntryBlockId(), var);
        }
    }
}

void SSA_Generator::traverseForVar(int blockId, const IRval &var) {
    auto &curBlock = cfg->block(blockId);

    // Phis
    for (auto &phiNode : curBlock.phis) {
        if (phiNode.res && *phiNode.res == var) {
            phiNode.res->version = versionsCnt;
            versions.push_back(versionsCnt++);
            break;
        }
    }

    // General nodes
    for (auto &node : curBlock.body) {
        for (IRval *arg : node.body->getArgs()) {
            if (*arg == var) {
                arg->version = versions.back();
            }
        }
        if (node.res && *node.res == var) {
            node.res->version = versionsCnt;
            versions.push_back(versionsCnt++);
        }
    }

    // Terminator
    if (curBlock.termNode.has_value()) {
        auto &terminator = dynamic_cast<IR_ExprTerminator &>(*curBlock.termNode->body);
        if (terminator.arg.has_value() && *terminator.arg == var) {
            terminator.arg->version = versions.back();
        }
    }

    for (int nextId : curBlock.next) {
        auto &nextBlock = cfg->block(nextId);
        int j = -1;
        for (size_t k = 0; k < nextBlock.prev.size(); k++) {
            if (nextBlock.prev.at(k) == blockId) {
                j = k;
                break;
            }
        }

        for (auto &phiNode : nextBlock.phis) {
            if (phiNode.res && *phiNode.res == var) {
                IRval phiArg = var;
                phiArg.version = versions.back();
                auto &phiExpr = dynamic_cast<IR_ExprPhi &>(*phiNode.body);
                phiExpr.args.emplace(j, phiArg);
                break;
            }
        }
    }

    for (int domChild : doms->getChildren(blockId))
        traverseForVar(domChild, var);

    // Rollback versions in stack
    if (curBlock.termNode.has_value()) {
        auto &terminator = dynamic_cast<IR_ExprTerminator &>(*curBlock.termNode->body);
        if (terminator.arg && *terminator.arg == var)
            versions.pop_back();
    }
    for (auto &node : curBlock.body)
        if (node.res && *node.res == var)
            versions.pop_back();
    for (auto &phiNode : curBlock.phis) {
        if (phiNode.res && *phiNode.res == var) {
            versions.pop_back();
            break;
        }
    }
}
