#ifndef LOOPS_DETECTOR_HPP_INCLUDED__
#define LOOPS_DETECTOR_HPP_INCLUDED__

#include <vector>
#include <set>
#include <map>
#include <memory>
#include "ir/unit.hpp"
#include "graph_info.hpp"

class LoopsDetector  {
public:
    struct LoopNode {
        IR_Block const &head;
        std::set<int> ends;
        std::set<int> blocks;

        LoopNode *parent = nullptr;
        std::vector<LoopNode*> children;

        LoopNode(IR_Block const &head);
    };

    LoopsDetector(CFGraph const &in);

    bool hasLoops() const;
    std::set<int> getBlockLoops(int blockId) const;
    std::map<int, LoopNode> const& getLoops() const;

    void printInfo() const;

private:
    struct BlockInfo {
        std::set<int> loops;
    };

    CFGraph const &cfg;
    GraphInfo gInfo;

    bool improperLoops = false;

    std::map<int, LoopNode> loops; // Loop ID is its head block ID
    std::map<int, BlockInfo> lblocks;

    void traverseLoopBranch(LoopNode &loop, int tailId);
    bool testLoopsRelation(LoopNode &sLoop, LoopNode &lLoop);
};

#endif /* LOOPS_DETECTOR_HPP_INCLUDED__ */
