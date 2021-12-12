#ifndef __CFG_HPP__
#define __CFG_HPP__

#include <vector>
#include <memory>
#include <map>

#include "nodes.hpp"


class ControlFlowGraph {
public:
    struct Function {
    private:
        int entryBlockId;
        friend class ControlFlowGraph;

    public:
        IR_StorageSpecifier storage;
        bool isInline;
        std::shared_ptr<IR_Type> fullType;

        Function clone() const;
    };


private:
    int blocksCounter = 0;
    uint64_t regs_counter = 0;

    std::map<int, IR_Block> blocks;
    std::vector<Function> funcs;

public:
    ControlFlowGraph() = default;

    ControlFlowGraph(ControlFlowGraph const &oth);

    IR_Block& createBlock();
    void linkBlocks(IR_Block &prev, IR_Block &next);
    IR_Block& createFunction(Function func);
    IRval createReg(std::shared_ptr<IR_Type> type);

    /** get block by id */
    IR_Block& block(int id);

    void printBlocks() const;
};

#endif /* __CFG_HPP__ */
