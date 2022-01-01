#include "dominators.hpp"
#include <algorithm>
#include <stack>
#include <sstream>

Dominators::DomNode::DomNode(Dominators &holder, int id)
        : holder(holder), id(id), w(std::make_unique<UtilData>()) {
    w->sdomId = id;
}

Dominators::DomNode& Dominators::DomNode::getAncestor() {
    return holder.data.at(holder.data.at(id).w->ancestorId);
}

Dominators::DomNode& Dominators::DomNode::getLabel() {
    return holder.data.at(holder.data.at(id).w->labelId);
}

Dominators::DomNode& Dominators::DomNode::getSdom() {
    return holder.data.at(holder.data.at(id).w->sdomId);
}

Dominators::DomNode &Dominators::DomNode::getParent() {
    return holder.data.at(holder.data.at(id).w->parentId);
}

int Dominators::DomNode::getTime() {
    return holder.data.at(id).w->time;
}

void Dominators::DomNode::bake() {
    w.reset();
}


Dominators::Dominators(std::shared_ptr<ControlFlowGraph> in_cfg) : cfg(std::move(in_cfg)) {
    std::vector<int> sortedIds;
    for (auto const &[blockId, block] : cfg->getBlocks()) {
        data.emplace(blockId, DomNode(*this, blockId));
        sortedIds.push_back(blockId);
    }

    for (const auto &[fid, func] : cfg->getFuncs()) {
        int entryId = func.getEntryBlockId();
        data.at(entryId).w->isEnter = true;
        dfs(entryId);

        // Sort nodes by DFS time
        std::sort(sortedIds.begin(), sortedIds.end(), [this](int a, int b) {
            return data.at(a).getTime() < data.at(b).getTime();
        });

        dominators(sortedIds);
        data.at(entryId).dominatorId = -1;

        // Clear visited flags
        for (auto const &[nodeId, node] : data) {
            node.w->visited = false;
        }
    }

    for (auto &[nodeId, node] : data) {
        if (node.dominatorId != -1)
            data.at(node.dominatorId).childrenIds.push_back(nodeId);
        node.bake();
    }
    cfg.reset();
}

void Dominators::dfs(int nodeId) {
    data.at(nodeId).w->visited = true;
    data.at(nodeId).w->time = globalTime++;
    for (int nextId : cfg->block(nodeId).next) {
        if (!data.at(nextId).w->visited) {
            data.at(nextId).w->parentId = nodeId;
            dfs(nextId);
        }
    }
}

int Dominators::findMin(int nodeId) {
    if (data.at(nodeId).w->ancestorId == -1)
        return nodeId;

    std::stack<int> stack;
    int uId = nodeId;
    while (data.at(uId).getAncestor().w->ancestorId != -1) {
        stack.push(uId);
        uId = data.at(uId).w->ancestorId;
    }
    while (!stack.empty()) {
        nodeId = stack.top();
        stack.pop();
        int lTime = data.at(nodeId).getAncestor().getLabel().getSdom().getTime();
        int rTime = data.at(nodeId).getLabel().getSdom().getTime();
        if (lTime < rTime)
            data.at(nodeId).w->labelId = data.at(nodeId).getAncestor().w->labelId;
        data.at(nodeId).w->ancestorId = data.at(uId).w->ancestorId;
    }
    return data.at(nodeId).w->labelId;
}

// NB: nodes must be sorted in ascending order
void Dominators::dominators(std::vector<int> &nodes) {
    for (auto &[nodeId, node] : data) {
        node.w->labelId = nodeId;
        node.w->sdomId = nodeId;
        node.w->ancestorId = -1;
    }

    for (ssize_t i = std::ssize(nodes) - 1; i >= 0; i--) { // Descending order
        auto &wNode = data.at(nodes.at(i));
        if (wNode.w->isEnter || !wNode.w->visited) // isEnter?
            continue;
        for (auto vId : cfg->block(wNode.id).prev) {
            if (!data.at(vId).w->visited)
                continue;
            auto &uNode = data.at(findMin(vId));
            if (uNode.getSdom().getTime() < wNode.getSdom().getTime())
                wNode.w->sdomId = uNode.w->sdomId;
        }
        wNode.w->ancestorId = wNode.w->parentId;
        wNode.getSdom().w->bucket.push_back(wNode.id);
        for (auto vId : wNode.getParent().w->bucket) {
            auto &vNode = data.at(vId);
            auto &uNode = data.at(findMin(vId));
            if (uNode.getSdom().id == vNode.getSdom().id)
                vNode.dominatorId = vNode.getSdom().id;
            else
                vNode.dominatorId = uNode.id;
        }
        wNode.getParent().w->bucket.clear();
    }
    for (ssize_t i = 0; i < std::ssize(nodes); i++) { // ascending order
        auto &wNode = data.at(nodes.at(i));
        if (wNode.w->isEnter || !wNode.w->visited) // isEnter?
            continue;
        if (wNode.dominatorId != wNode.getSdom().id)
            wNode.dominatorId = data.at(wNode.dominatorId).dominatorId;
    }
}


// Getters

bool Dominators::isIdom(int x, int y) const {
    return data.at(y).dominatorId == x;
}

bool Dominators::isDom(int x, int y) const {
    int curId = y;
    while (curId != -1) {
        if (curId == x)
            return true;
        curId = data.at(curId).dominatorId;
    }
    return false;
}

bool Dominators::isSdom(int x, int y) const {
    return x != y && isDom(x, y);
}

int Dominators::getIdom(int x) const {
    return data.at(data.at(x).dominatorId).id;
}

std::vector<int> Dominators::getChildren(int x) const {
    return data.at(x).childrenIds;
}


std::string Dominators::drawGraph() const {
    std::ostringstream ss;
    ss << "digraph {" << std::endl;
    for (auto const &[nodeId, node] : data) {
        if (node.dominatorId != -1) {
            ss << node.dominatorId << "->";
            ss << nodeId << std::endl;
        }
    }
    ss << "}";
    return ss.str();
}
