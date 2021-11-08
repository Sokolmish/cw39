#ifndef __PRINT_TREE_HPP__
#define __PRINT_TREE_HPP__

#include <string>
#include <sstream>
#include <vector>
#include <memory>

class TreeNode {
private:
    std::string name;
    std::vector<std::shared_ptr<TreeNode>> children;

    void printVertRec(std::stringstream &ss, std::vector<bool> &prev) const;
    void printHorRec(std::stringstream &ss, std::vector<int> &prev) const;

public:
    explicit TreeNode(std::string name);
    static std::shared_ptr<TreeNode> create(const std::string &name);

    void setName(const std::string &name);
    void addChild(const std::shared_ptr<TreeNode>& child);

    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::vector<std::shared_ptr<TreeNode>> getChildren() const;

    [[nodiscard]] std::string printVert() const;
    [[nodiscard]] std::string printHor() const;
};

typedef std::shared_ptr<TreeNode> TreeNodeRef;

#endif /* __PRINT_TREE_HPP__ */
