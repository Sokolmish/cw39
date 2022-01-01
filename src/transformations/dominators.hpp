#ifndef __DOMINATORS_HPP__
#define __DOMINATORS_HPP__

#include <vector>
#include <map>
#include <string>
#include "ir/cfg.hpp"

class Dominators {
private:
    class DomNode {
    private:
        struct UtilData {
            bool visited = false;
            int time;
            int parentId, sdomId, labelId;
            int ancestorId = -1;
            std::vector<int> bucket; // Of ids
            bool isEnter = false;
        };

        Dominators &holder;

    public:
        int id;
        int dominatorId = -1;
        std::vector<int> childrenIds;
        std::unique_ptr<UtilData> w;

        DomNode(Dominators &holder, int id);

        DomNode& getAncestor();
        DomNode& getLabel();
        DomNode& getSdom();
        DomNode& getParent();
        int getTime();

        /** Erase temporary working data */
        void bake();
    };

    std::shared_ptr<ControlFlowGraph> cfg;
    int globalTime = 1;
    std::map<int, DomNode> data;

    void dfs(int nodeId);
    int findMin(int nodeId);
    void dominators(std::vector<int> &nodes);

public:
    explicit Dominators(std::shared_ptr<ControlFlowGraph> in_cfg);

    /** Is immediate dominator */
    bool isIdom(int x, int y) const;

    /** Is weak dominator */
    bool isDom(int x, int y) const;

    /** Is strong dominator (not semidominator) */
    bool isSdom(int x, int y) const;

    /** Get immediate dominator */
    int getIdom(int x) const;

    std::vector<int> getChildren(int x) const;

    std::string drawGraph() const;
};

#endif /* __DOMINATORS_HPP__ */
