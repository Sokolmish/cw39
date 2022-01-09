#ifndef __SSA_GENERATOR_HPP__
#define __SSA_GENERATOR_HPP__

#include <map>
#include <set>
#include <deque>
#include <memory>
#include "ir/cfg.hpp"
#include "transformations/dominators.hpp"

class SSA_Generator {
private:
    std::shared_ptr<ControlFlowGraph> origCfg;
    std::shared_ptr<ControlFlowGraph> cfg;

    std::unique_ptr<Dominators> doms;
    std::map<int, int> postOrder;
    std::map<int, std::set<int>> verticesDF;

    int versionsCnt = -1;
    std::deque<int> versions;

    void placePhis();
    void makePostOrder(std::set<int> &visited, int &counter, int cur);
    /** Get dominance frontier for each single variable */
    void makeVerticesDF();
    std::set<int> getSetDF(const std::set<int> &S) const;
    std::set<int> getSetDFP(const std::set<int> &S) const;

    void versionize();
    void traverseForVar(int blockId, IRval const &var);

public:
    explicit SSA_Generator(std::shared_ptr<ControlFlowGraph> in_cfg);

    std::shared_ptr<ControlFlowGraph> getCfg();
};


#endif /* __SSA_GENERATOR_HPP__ */
