#ifndef IR_UNIT_HPP_INCLUDED__
#define IR_UNIT_HPP_INCLUDED__

#include <vector>
#include <memory>
#include <map>
#include <set>
#include <functional>
#include "nodes.hpp"


class IntermediateUnit;

class CFGraph {
public:
    CFGraph() = default;
    CFGraph(CFGraph &&) noexcept = default;
    CFGraph& operator=(CFGraph &&) noexcept = default;

    CFGraph copy() const;

    IR_Block& createBlock();
    IR_Block& insertBlock(IR_Block block);
    void linkBlocks(IR_Block &prev, IR_Block &next);
    void linkBlocks(int prevId, int nextId);

    /** get block by id */
    IR_Block& block(int id);
    IR_Block const& block(int id) const;

    std::map<int, IR_Block> const& getBlocks() const;
    std::map<int, IR_Block>& getBlocksData();

    void traverseBlocks(int blockId, std::set<int> &visited,
                        std::function<void(int)> const &action) const;

    IRval createReg(std::shared_ptr<IR_Type> type);

    int entryBlockId = -1;

private:
    uint64_t regsCounter = 0;
    int blocksCounter = 0;

    std::map<int, IR_Block> blocks;

//    CFGraph(CFGraph const &) = default;
//    CFGraph& operator=(CFGraph const &) = default;
};

class IntermediateUnit {
public:
    enum class FunLinkage { EXTERN, STATIC, WEAK };
    enum class VarLinkage { DEFAULT, EXTERN, STATIC, TENTATIVE, WEAK };

    class Function;

    struct GlobalVar {
        int id;
        std::string name;
        std::shared_ptr<IR_Type> type;
        IRval init;
        VarLinkage storage;
    };

    IntermediateUnit() = default;
    IntermediateUnit(IntermediateUnit const &oth);
    IntermediateUnit(IntermediateUnit&&) noexcept = default;
    IntermediateUnit& operator=(IntermediateUnit&&) noexcept = default;

    Function& createFunction(std::string name, FunLinkage stor, int fspec, std::shared_ptr<IR_Type> fullType);
    Function& createPrototype(std::string name, FunLinkage stor, std::shared_ptr<IR_Type> fullType);
    IRval createGlobal(std::string name, std::shared_ptr<IR_Type> type, IRval init, VarLinkage stor);

    Function& getFunction(int id);
    Function const& getFunction(int id) const;

    uint64_t putString(std::string str);

    std::map<int, Function> const& getFuncs() const;
    std::map<int, Function>& getFuncsMut();
    std::map<int, Function> const& getPrototypes() const;
    std::map<int, GlobalVar> const& getGlobals() const;
    std::map<string_id_t, std::shared_ptr<IR_TypeStruct>> const& getStructs() const;
    std::map<uint64_t, std::string> const& getStrings() const;

    [[nodiscard]] std::string printIR() const;
    [[nodiscard]] std::string drawCFG() const;

private:
    int funcsCounter = 0;
    uint64_t stringsCounter = 0;
    int globalsCounter = 0;

    std::map<int, Function> funcs;
    std::map<int, Function> prototypes;
    std::map<string_id_t, std::shared_ptr<IR_TypeStruct>> structs;
    std::map<uint64_t, std::string> strings;
    std::map<int, GlobalVar> globals;

    friend class IR_Generator;
    friend class CFGraph;

    void printExpr(std::stringstream &ss, IR_Expr const &rawExpr) const;
    void printBlock(std::stringstream &ss, IR_Block const &block) const;
    void drawBlock(std::stringstream &ss, IR_Block const &block) const;
};

class IntermediateUnit::Function {
public:
    FunLinkage storage;
    std::shared_ptr<IR_Type> fullType;

    enum FuncSpec : int {
        FSPEC_NONE   = 0,
        FSPEC_INLINE = 0x1,
        FSPEC_FCONST = 0x2,
        FSPEC_PURE   = 0x4,
        FSPEC_GOTOED = 0x8,
    };
    int fspec;

    CFGraph cfg;

    bool isProto;

    Function(int id, std::string name, int isProto = false);
    Function(Function const &oth);

    Function(Function&&) noexcept = default;
    Function& operator=(Function&&) noexcept = default;

    int getId() const;
    std::string getName() const;
    std::shared_ptr<IR_TypeFunc> getFuncType() const;
    bool isPure() const;
    bool isInline() const;

private:
    int id;
    std::string name;
    friend class IntermediateUnit;
};

#endif /* IR_UNIT_HPP_INCLUDED__ */
