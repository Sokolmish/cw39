#include "print_tree.hpp"
#include <stack>
#include <utility>

TreeNode::TreeNode(std::string name) : name(std::move(name)) {}

std::shared_ptr<TreeNode> TreeNode::create(const std::string &name) {
    return std::make_shared<TreeNode>(name);
}


void TreeNode::setName(const std::string &str) {
    this->name = str;
}

void TreeNode::addChild(const std::shared_ptr<TreeNode>& child) {
    children.push_back(child);
}


std::string TreeNode::getName() const {
    return name;
}

std::vector<std::shared_ptr<TreeNode>> TreeNode::getChildren() const {
    return children;
}


std::string TreeNode::printVert() const {
    std::stringstream ss;
    std::vector<bool> prev;
    prev.push_back(true);
    printVertRec(ss, prev);
    return ss.str();
}

std::string TreeNode::printHor() const {
    std::stringstream ss;
    std::vector<int> prev;
    prev.push_back(0);
    printHorRec(ss, prev);
    return ss.str();
}

void TreeNode::printVertRec(std::stringstream &ss, std::vector<bool> &prev) const {        
    int indent = (int)(prev.size()) - 1;
    if (indent != 0) {
        for (int i = 1; i < indent; i++)
            ss << (prev[i] ? "  " : "│ ");
        if (prev.back())
            ss << "└─" << name << std::endl;
        else
            ss << "├─" << name << std::endl;
    }
    else {
        ss << name << std::endl;
    }
    if (!children.empty()) {
        prev.push_back(false);
        for (int i = 0; i < (int)(children.size()) - 1; i++)
            children[i]->printVertRec(ss, prev);
        prev[prev.size() - 1] = true;
        children[children.size() - 1]->printVertRec(ss, prev);
        prev.pop_back();
    }
}

void TreeNode::printHorRec(std::stringstream &ss, std::vector<int> &prev) const {
    if (prev.back() != 0) {
        for (int i = 1; i < (int)(prev.size()) - 1; i++) {
            for (int j = 0; j < abs(prev[i]); j++)
                ss << " ";
            ss << ((prev[i] < 0) ? "  " : "│ ");
        }
        
        for (int j = 0; j < abs(prev.back()); j++)
            ss << " ";
        ss << ((prev.back() < 0) ? "└─" : "├─");
    }

    if (!children.empty()) {
        std::stack<const TreeNode*> stack;
        stack.push(this);

        const TreeNode *cur = stack.top();
        while (!cur->children.empty()) {
            ss << cur->name;
            if (cur->children.size() == 1) {
                ss << "───";
                prev.push_back(-((int)cur->name.size() + 1));
            }
            else {
                ss << "─┬─";
                prev.push_back((int)cur->name.size() + 1);
            }

            stack.push(cur->children.front().get());
            cur = stack.top();
        }
        
        ss << cur->name << std::endl;
        prev.push_back(0);

        while (!stack.empty()) {
            int childrenCount = (int)stack.top()->children.size();
            if (childrenCount > 1) {
                for (int i = 1; i < childrenCount - 1; i++)
                    stack.top()->children[i]->printHorRec(ss, prev);
                prev[prev.size() - 1] *= -1;
                stack.top()->children[childrenCount - 1]->printHorRec(ss, prev);
            }

            stack.pop();
            prev.pop_back();
        }
    }
    else {
        ss << name << std::endl;
    }
}
