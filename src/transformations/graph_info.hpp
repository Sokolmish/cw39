#ifndef __GRAPH_INFO_HPP__
#define __GRAPH_INFO_HPP__

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "ir/cfg.hpp"

class GraphInfo {
public:
    explicit GraphInfo(std::shared_ptr<ControlFlowGraph> graph);

    enum ArcClass { NONE, TREE, BACK, FORWARD, CROSS };
    ArcClass getArcClass(int from, int to);

    /** Is x immediate dominator of y */
    bool isIdom(int x, int y) const;
    /** Is x weak dominator of y */
    bool isDom(int x, int y) const;
    /** Is x strong dominator of y (not semidominator) */
    bool isSdom(int x, int y) const;
    /** Get immediate dominator */
    int getIdom(int x) const;
    /** Get nodes immediate dominated by x */
    std::vector<int> getChildren(int x) const;

    std::string drawArcsClasses() const;
    std::string drawDominators() const;

private:
    // Graph arcs
    std::map<std::pair<int, int>, GraphInfo::ArcClass> arcsClasses;

    // Dominators tree (bidirectional)
    struct DomNode {
        int dominatorId = -1;
        std::vector<int> childrenIds;
    };
    std::map<int, DomNode> domData;

    // Temporary data (used only at construction)
    std::shared_ptr<ControlFlowGraph> cfg;
    int globalTime = 1;

    struct UtilNode {
        int id;

        int timeIn = -1;
        int timeOut = -1;

        UtilNode *parent = nullptr;
        UtilNode *ancestor = nullptr;
        UtilNode *sdom;
        UtilNode *label;

        std::vector<UtilNode*> bucket;

        DomNode *dom;

        UtilNode(int id, DomNode *dom);
    };
    std::map<int, UtilNode> utilNodes;

    // Unprocessed data
    std::vector<int> funBlocksIds; // Current function's blocks
    std::vector<std::pair<UtilNode*, UtilNode*>> unprocArcs;

    void dfs(UtilNode &node);
    void processFunc(int entryBlockId);
    void classifyArcs();

    UtilNode& findMin(UtilNode &node);
    void dominators(std::vector<int> &nodes);
};

#endif /* __GRAPH_INFO_HPP__ */
