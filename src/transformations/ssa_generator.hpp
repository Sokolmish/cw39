#ifndef SSA_GENERATOR_HPP_INCLUDED__
#define SSA_GENERATOR_HPP_INCLUDED__

#include <map>
#include <set>
#include <deque>
#include <memory>
#include "graph_info.hpp"
#include "ir_transformer.hpp"

class SSA_Generator : IRTransformer {
private:
    ControlFlowGraph cfg;

    std::unique_ptr<GraphInfo> gInfo;
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
    explicit SSA_Generator(ControlFlowGraph in_cfg);

    ControlFlowGraph const& getCfg() override;
    ControlFlowGraph moveCfg() && override;
};


#endif /* SSA_GENERATOR_HPP_INCLUDED__ */
