#include "graph_info.hpp"
#include <stack>
#include <sstream>
#include <fmt/ostream.h>
#include "utils.hpp"

GraphInfo::UtilNode::UtilNode(int id, DomNode *dom) : id(id), dom(dom) {}

GraphInfo::GraphInfo(ControlFlowGraph const &graph) : cfg(graph) {
    for (auto const &[fid, func] : cfg.getFuncs()) {
        processFunc(func.getEntryBlockId());
    }
    for (auto &[nodeId, node] : domData) {
        if (node.dominatorId != -1)
            domData.at(node.dominatorId).childrenIds.push_back(nodeId);
    }
}


void GraphInfo::processFunc(int entryBlockId) {
    auto domIt = domData.emplace(entryBlockId, DomNode()).first;
    UtilNode node = UtilNode(entryBlockId, &domIt->second);
    auto utilIt = utilNodes.emplace(entryBlockId, std::move(node)).first;
    UtilNode &newNode = utilIt->second;
    dfs(newNode);

    classifyArcs();

    std::sort(funBlocksIds.begin(), funBlocksIds.end(), [this](int a, int b) {
        return utilNodes.at(a).timeIn < utilNodes.at(b).timeIn;
    });
    dominators(funBlocksIds);
    domData.at(entryBlockId).dominatorId = -1;

    funBlocksIds.clear();
    utilNodes.clear();
}

void GraphInfo::dfs(UtilNode &node) {
    funBlocksIds.push_back(node.id);
    node.timeIn = globalTime++;

    IR_Block const &block = cfg.block(node.id);
    for (int nextId : block.next) {
        auto it = utilNodes.lower_bound(nextId);
        if (it == utilNodes.end() || it->first != nextId) {
            arcsClasses.emplace(std::make_pair(node.id, nextId), GraphInfo::TREE);

            auto &domNode = domData.emplace(nextId, DomNode()).first->second;
            auto &newNode = utilNodes.emplace_hint(it, nextId, UtilNode(nextId, &domNode))->second;

            newNode.parent = &node;
            dfs(newNode);
        }
        else {
            unprocArcs.emplace_back(&node, &it->second);
        }
    }
    node.timeOut = globalTime++;
}

void GraphInfo::classifyArcs() {
    for (auto &[from, to] : unprocArcs) {
        if (from->timeIn < to->timeIn && to->timeOut < from->timeOut)
            arcsClasses.emplace(std::make_pair(from->id, to->id), GraphInfo::FWD);
        else if (to->timeIn < from->timeIn && from->timeOut < to->timeOut)
            arcsClasses.emplace(std::make_pair(from->id, to->id), GraphInfo::BACK);
        else // ? to->timeOut < from->timeIn
            arcsClasses.emplace(std::make_pair(from->id, to->id), GraphInfo::CROSS);
    }
    unprocArcs.clear();
}

GraphInfo::UtilNode& GraphInfo::findMin(UtilNode &node) {
    if (node.ancestor == nullptr)
        return node;

    std::stack<UtilNode*> stack;
    UtilNode *uNode = &node;
    while (uNode->ancestor->ancestor != nullptr) {
        stack.push(uNode);
        uNode = uNode->ancestor;
    }

    UtilNode *curNode = &node;
    UtilNode *lastLabel = curNode->label; // Suppress CLion bug
    while (!stack.empty()) {
        curNode = stack.top();
        int lTime = curNode->ancestor->label->sdom->timeIn;
        int rTime = curNode->label->sdom->timeIn;
        if (lTime < rTime)
            curNode->label = curNode->ancestor->label;
        curNode->ancestor = uNode->ancestor;
        lastLabel = curNode->label;
        stack.pop();
    }
    return *lastLabel;
}

// NB: nodes must be sorted in ascending order
void GraphInfo::dominators(std::vector<int> &nodes) {
    for (auto &[nodeId, node] : utilNodes) {
        node.label = &node;
        node.sdom = &node;
    }

    for (auto it = nodes.rbegin(); it != nodes.rend(); ++it) {
        auto &wNode = utilNodes.at(*it);
        if (wNode.parent == nullptr || wNode.timeIn == -1)
            continue;

        for (auto vId : cfg.block(wNode.id).prev) {
            UtilNode &vNode = utilNodes.at(vId);
            if (vNode.timeIn == -1)
                continue;
            auto &uNode = findMin(vNode);
            if (uNode.sdom->timeIn < wNode.sdom->timeIn)
                wNode.sdom = uNode.sdom;
        }
        wNode.ancestor = wNode.parent;
        wNode.sdom->bucket.push_back(&wNode);

        auto &bucket = wNode.parent->bucket;
        for (UtilNode *vNode : bucket) {
            auto &uNode = findMin(*vNode);
            if (uNode.sdom == vNode->sdom)
                vNode->dom->dominatorId = vNode->sdom->id;
            else
                vNode->dom->dominatorId = uNode.id;
        }
        bucket.clear();
    }

    for (auto it = nodes.begin(); it != nodes.end(); ++it) { // ascending order
        auto &wNode = utilNodes.at(*it);
        if (wNode.parent == nullptr || wNode.timeIn == -1)
            continue;
        if (wNode.dom->dominatorId != wNode.sdom->id)
            wNode.dom->dominatorId = domData.at(wNode.dom->dominatorId).dominatorId;
    }
}


// Getters

GraphInfo::ArcClass GraphInfo::getArcClass(int from, int to) {
    auto it = arcsClasses.find(std::make_pair(from, to));
    return it == arcsClasses.end() ? GraphInfo::NONE : it->second;
}

std::vector<std::pair<int, int>> GraphInfo::getArcs(GraphInfo::ArcClass cl) {
    std::vector<std::pair<int, int>> res;
    for (auto const &[arc, aCl] : arcsClasses)
        if (aCl == cl)
            res.push_back(arc);
    return res;
}

bool GraphInfo::isIdom(int x, int y) const {
    return domData.at(y).dominatorId == x;
}

bool GraphInfo::isDom(int x, int y) const {
    int curId = y;
    while (curId != -1) {
        if (curId == x)
            return true;
        curId = domData.at(curId).dominatorId;
    }
    return false;
}

bool GraphInfo::isSdom(int x, int y) const {
    return x != y && isDom(x, y);
}

int GraphInfo::getIdom(int x) const {
    return domData.at(x).dominatorId;
}

std::vector<int> GraphInfo::getChildren(int x) const {
    return domData.at(x).childrenIds;
}


// Printers

std::string GraphInfo::drawDominators() const {
    std::ostringstream ss;
    ss << "digraph {" << std::endl;
    for (auto const &[nodeId, node] : domData) {
        if (node.dominatorId != -1) {
            ss << node.dominatorId << "->";
            ss << nodeId << std::endl;
        }
    }
    ss << "}";
    return ss.str();
}

std::string GraphInfo::drawArcsClasses() const {
    std::stringstream ss;
    fmt::print(ss, "digraph{{ ");
    for (auto &[arc, cl] : arcsClasses) {
        std::string fstr;
        switch (cl) {
            case GraphInfo::NONE:
                fstr = "{}->{}[style=\"dotted,bold\",color=purple]; ";
                break;
            case GraphInfo::TREE:
                fstr = "{}->{}[style=\"bold\",color=black]; ";
                break;
            case GraphInfo::BACK:
                fstr = "{}->{}[style=\"solid\",color=darkred,constraint=false]; ";
                break;
            case GraphInfo::CROSS:
                fstr = "{}->{}[style=\"dashed\",color=darkblue,constraint=false]; ";
                break;
            case GraphInfo::FWD:
                fstr = "{}->{}[style=\"dashed,bold\",color=darkgreen,constraint=false]; ";
                break;
            default:
                internalError("Smth went wrong");
        }
        fmt::print(ss, fstr, arc.first, arc.second);
    }
    fmt::print(ss, "}}\n");
    return ss.str();
}
