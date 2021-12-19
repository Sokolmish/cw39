#ifndef __CFG_HPP__
#define __CFG_HPP__

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <functional>
#include "nodes.hpp"


class ControlFlowGraph {
public:
    class Function {
    private:
        int id;
        int entryBlockId;
        friend class ControlFlowGraph;

    public:
        IR_StorageSpecifier storage;
        bool isInline;
        std::shared_ptr<IR_Type> fullType;

        Function clone() const;
        int getId() const;
        int getEntryBlockId() const;
        IR_TypeFunc const& getFuncType() const;
    };

    struct GlobalVar {
        IRval self;
        IRval val;
    };


private:
    int blocksCounter = 0;
    uint64_t regs_counter = 0;
    int funcsCounter = 0;
    uint64_t stringsCounter = 0;
    uint64_t globalsCounter = 0;

    std::map<int, IR_Block> blocks;
    std::map<int, Function> funcs;
    std::map<string_id_t, std::shared_ptr<IR_TypeStruct>> structs;
    std::map <uint64_t, std::string> strings;
    std::map <uint64_t, GlobalVar> globals;

    friend class IR_Generator;

public:
    ControlFlowGraph() = default;
    ControlFlowGraph(ControlFlowGraph const &oth);

    IR_Block& createBlock();
    void linkBlocks(IR_Block &prev, IR_Block &next);
    Function& createFunction(IR_StorageSpecifier stor, bool isInline, std::shared_ptr<IR_Type> fullType);
    IRval createReg(std::shared_ptr<IR_Type> type);

    /** get block by id */
    IR_Block& block(int id);
    Function& getFunction(int id);
    IRval getGlobalSelf(uint64_t id);

    uint64_t putString(std::string str);
    uint64_t putGlobal(IRval val);

    std::map<int, Function> const& getFuncs() const;
    std::map<int, IR_Block> const& getBlocks() const;
    std::map <uint64_t, GlobalVar> const& getGlobals() const;

    void traverseBlocks(int blockId, std::set<int> &visited, std::function<void(int)> action);

    void printCFG() const;
};

#endif /* __CFG_HPP__ */
