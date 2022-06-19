#include "ssa_generator.hpp"
#include "cfg_cleaner.hpp"
#include <vector>
#include <stack>

SSA_Generator::SSA_Generator(IntermediateUnit const &unit, CFGraph in_cfg) : IRTransformer(std::move(in_cfg)) {
    gInfo = std::make_unique<GraphInfo>(cfg);

    placePhis();
    versionize();

    gInfo.reset();

    CfgCleaner cleaner(unit, std::move(cfg));
    cleaner.removeUselessNodes();
    cleaner.removeNops();
    if (cleaner.isPassEffective())
        setPassChanged();
    cfg = std::move(cleaner).moveCfg();
}

// PHIs placing

void SSA_Generator::placePhis() {
    int counter = 0;
    std::set<int> visited;
    makePostOrder(visited, counter, cfg.entryBlockId);

    makeVerticesDF();

    // For each variable find, where a value assigned to it
    std::map<IRval, std::set<int>> varsDefSet;
    for (auto const &[bId, block] : cfg.getBlocks())
        for (IRval const &def : block.getDefinitions())
            varsDefSet[def].insert(bId);

    for (auto const &[var, varDefs] : varsDefSet) {
        std::set<int> JP = getSetDFP(varDefs); // JP == DFP (theorem)
        for (int blockId : JP) {
            cfg.block(blockId).addNewPhiNode(var);
        }
    }
}

void SSA_Generator::makePostOrder(std::set<int> &visited, int &counter, int cur) {
    visited.insert(cur);
    for (int nextId : cfg.block(cur).next)
        if (!visited.contains(nextId))
            makePostOrder(visited, counter, nextId);
    postOrder.insert({ cur, counter++ });
}

void SSA_Generator::makeVerticesDF() {
    // Sort blocks by post order
    std::vector<int> sortedIds;
    for (auto const &[bId, block] : cfg.getBlocks())
        if (postOrder.contains(bId)) // TODO: get set from makePostOrder
            sortedIds.push_back(bId);
    std::sort(sortedIds.begin(), sortedIds.end(), [this](int a, int b) {
        return postOrder.at(a) < postOrder.at(b);
    });

    for (int xId : sortedIds) {
        auto it = verticesDF.emplace(xId, std::set<int>());
        auto &curSet = it.first->second;
        for (int yId : cfg.block(xId).next) {
            if (gInfo->getIdom(yId) != xId) {
                curSet.insert(yId);
            }
        }
        for (int z : gInfo->getChildren(xId)) {
            for (int y : verticesDF.at(z)) {
                if (gInfo->getIdom(y) != xId) {
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


// Versioning

void SSA_Generator::versionize() {
    // Collect variables from graph because its was not passed in CFG
    std::set<IRval> variables;
    for (auto const &[bId, block] : cfg.getBlocks()) {
        for (const IRval& def : block.getDefinitions())
            variables.insert(def);
        for (const IRval& ref : block.getReferences())
            variables.insert(ref);
    }

    for (const IRval& var : variables) {
        traverseForVar(cfg.entryBlockId, var);
    }
}

void SSA_Generator::traverseForVar(int startBlockId, const IRval &var) {
    std::stack<IRval> versions; // Maybe reuse this stack for different variables?
    versions.push(IRval::createUndef(var.getType())); // In case of uninitialized variable

    // A phis results
    std::set<IRval> phiRess;
    phiRess.insert(var); // Before versioning all phis retruns var

    enum { SSAV_REC_CALL = false, SSAV_ROLLBACK = true };
    std::stack<std::pair<int, bool>> stack;
    stack.push(std::make_pair(startBlockId, SSAV_REC_CALL));

    while (!stack.empty()) {
        if (stack.top().second == SSAV_ROLLBACK) {
            int rollback = stack.top().first;
            while (rollback--) {
                versions.pop();
            }
            stack.pop();
            continue;
        }

        int blockId = stack.top().first;
        stack.pop();

        auto &curBlock = cfg.block(blockId);
        int rollbackCnt = 0;

        // Phis
        for (auto &phiNode : curBlock.phis) {
            if (phiNode.res == var) {
                IRval rg = cfg.createReg(var.getType());
                phiNode.res = rg;
                setPassChanged();

                versions.push(std::move(rg));
                phiRess.insert(*phiNode.res);
                rollbackCnt++;
                break;
            }
        }

        // General nodes
        for (auto &node : curBlock.body) {
            for (IRval *arg : node.body->getArgs()) {
                if (*arg == var) {
                    IRval v = versions.top(); // CLion warnings...
                    *arg = std::move(v);
                    setPassChanged();
                }
            }
            if (node.res == var) {
                IRval rg = cfg.createReg(var.getType());
                node.res = rg;
                setPassChanged();

                versions.push(std::move(rg));
                rollbackCnt++;
            }
        }

        // Terminator
        if (curBlock.termNode.has_value()) {
            auto &terminator = dynamic_cast<IR_ExprTerminator &>(*curBlock.termNode->body);
            if (terminator.arg == var) {
                terminator.arg = versions.top();
                setPassChanged();
            }
        }

        // Set phis args in next blocks
        for (int nextId : curBlock.next) {
            auto &nextBlock = cfg.block(nextId);
            int j = -1;
            for (size_t k = 0; k < nextBlock.prev.size(); k++) {
                if (nextBlock.prev.at(k) == blockId) {
                    j = static_cast<int>(k);
                    break;
                }
            }

            for (auto &phiNode : nextBlock.phis) {
                if (phiNode.res && phiRess.contains(*phiNode.res)) {
                    IRval phiArg = versions.top();
                    auto &phiExpr = dynamic_cast<IR_ExprPhi &>(*phiNode.body);
                    phiExpr.args.emplace(j, phiArg);
                    setPassChanged();
                    break;
                }
            }
        }

        stack.push({ rollbackCnt, SSAV_ROLLBACK });
        for (int domChild : gInfo->getChildren(blockId))
            stack.push({ domChild, SSAV_REC_CALL });
    }
}
