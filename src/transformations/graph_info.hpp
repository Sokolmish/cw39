#ifndef GRAPH_INFO_HPP_INCLUDED__
#define GRAPH_INFO_HPP_INCLUDED__

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "ir/unit.hpp"

class GraphInfo {
public:
    explicit GraphInfo(CFGraph const &graph);

    enum ArcClass { NONE, TREE, BACK, FWD, CROSS };
    ArcClass getArcClass(int from, int to);

    std::vector<std::pair<int, int>> getArcs(ArcClass cl);

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

    [[nodiscard]] std::string drawArcsClasses() const;
    [[nodiscard]] std::string drawDominators() const;

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
    CFGraph const &cfg;
    int globalTime = 1;

    struct UtilNode {
        int id;

        int timeIn = -1;
        int timeOut = -1;

        UtilNode *parent = nullptr;
        UtilNode *ancestor = nullptr;
        UtilNode *sdom = nullptr;
        UtilNode *label = nullptr;

        std::vector<UtilNode*> bucket;

        DomNode *dom;

        UtilNode(int id, DomNode *dom);
    };
    std::map<int, UtilNode> utilNodes;

    // Unprocessed data
    std::vector<int> funBlocksIds; // Current function's blocks
    std::vector<std::pair<UtilNode*, UtilNode*>> unprocArcs;

    void dfs(UtilNode &node);
    void classifyArcs();

    UtilNode& findMin(UtilNode &node);
    void dominators(std::vector<int> &nodes);
};

#endif /* GRAPH_INFO_HPP_INCLUDED__ */
