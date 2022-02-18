#ifndef LOOPS_HPP_INCLUDED__
#define LOOPS_HPP_INCLUDED__

#include <vector>
#include <set>
#include <map>
#include <memory>
#include "ir/cfg.hpp"
#include "graph_info.hpp"

class Looper {
public:
    Looper(std::shared_ptr<ControlFlowGraph> in);

    void draw() const;

private:
    std::shared_ptr<ControlFlowGraph> cfg;
    GraphInfo gInfo;
    std::map<int, std::vector<int>> spanningTree;

    struct LoopNode {
        IR_Block &block;
        LoopNode *parent;
        std::set<int> ends;

        LoopNode(IR_Block &bb);
    };
    std::map<int, LoopNode> loops;

    struct BlockInfo {
        IR_Block &block;
        LoopNode *loop = nullptr;

        BlockInfo(IR_Block &bb);
    };
    std::map<int, BlockInfo> lblocks;

    void dfs1(int nodeId, LoopNode *curLoop);
    void dfs2(int nodeId, std::vector<int> path, LoopNode *curLoop, std::set<int> ends);
    bool isLoopNested(const LoopNode *parent, const LoopNode *child) const;
};

#endif /* LOOPS_HPP_INCLUDED__ */
