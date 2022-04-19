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

        LoopNode(IR_Block const &head);
    };

    struct BlockInfo {
        std::set<int> loops;
    };

    std::map<int, LoopNode> loops;
    std::map<int, BlockInfo> lblocks;

    void traverseLoopBranch(LoopNode &loop, int tailId);
    bool testLoopsRelation(LoopNode &sLoop, LoopNode &lLoop);
};

#endif /* LOOPS_HPP_INCLUDED__ */
