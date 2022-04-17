#ifndef LOOPS_HPP_INCLUDED__
#define LOOPS_HPP_INCLUDED__

#include <vector>
#include <set>
#include <map>
#include <memory>
#include "ir/unit.hpp"
#include "graph_info.hpp"

class Looper {
public:
    Looper(CFGraph const &in);

    void draw() const;

private:
    CFGraph const &cfg;
    GraphInfo gInfo;

    struct LoopNode {
        IR_Block const &head;
        LoopNode *parent;
        std::set<int> ends;

        std::set<int> blocks;

        LoopNode(IR_Block const &bb);
    };
    std::map<int, LoopNode> loops;

    struct BlockInfo {
        IR_Block const &block;
        LoopNode *loop = nullptr;

        BlockInfo(IR_Block const &bb);
    };
    std::map<int, BlockInfo> lblocks;

    void findLoopsBodies();
    std::map<int, std::vector<int>> straightPaths;
    void dfs3(LoopNode &loop, int nodeId, std::vector<int> curPath);

    void findLoopsInNormalFunc(IntermediateUnit::Function const &func);
};

#endif /* LOOPS_HPP_INCLUDED__ */
