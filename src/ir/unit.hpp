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
    explicit CFGraph(IntermediateUnit *iunit);

    CFGraph(CFGraph &&) noexcept = default;
    CFGraph& operator=(CFGraph &&) noexcept = default;

    CFGraph copy(IntermediateUnit *iunit) const;

    IR_Block& createBlock();
    void linkBlocks(IR_Block &prev, IR_Block &next);
    void linkBlocks(int prevId, int nextId);

    /** get block by id */
    IR_Block& block(int id);
    IR_Block const& block(int id) const;

    std::map<int, IR_Block> const& getBlocks() const;
    std::map<int, IR_Block>& getBlocksData();

    IntermediateUnit* getParentUnit();

    void traverseBlocks(int blockId, std::set<int> &visited,
                        std::function<void(int)> const &action) const;

    [[nodiscard]] std::string drawCFG() const;

    int entryBlockId = -1;

private:
    IntermediateUnit *par;

    std::map<int, IR_Block> blocks;

//    CFGraph(CFGraph const &) = default;
//    CFGraph& operator=(CFGraph const &) = default;
};

class IntermediateUnit {
public:
    enum class Linkage { EXTERN, STATIC, WEAK };

    class Function {
    public:
        Linkage storage;
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

        Function(IntermediateUnit *iunit, int id, std::string name, int isProto = false);
        Function(IntermediateUnit *iunit, Function const &oth);
        Function(Function &&) noexcept = default;

        int getId() const;
        std::string getName() const;
        std::shared_ptr<IR_TypeFunc> getFuncType() const;

    private:
        int id;
        std::string name;
        friend class IntermediateUnit;
    };

    struct GlobalVar {
        int id;
        std::string name;
        std::shared_ptr<IR_Type> type;
        IRval init;
    };

    IntermediateUnit() = default;
    IntermediateUnit(IntermediateUnit const &oth);
    IntermediateUnit(IntermediateUnit &&oth) noexcept = default;
    IntermediateUnit& operator=(IntermediateUnit &&oth) noexcept = default;

    Function& createFunction(std::string name, Linkage stor, int fspec, std::shared_ptr<IR_Type> fullType);
    Function& createPrototype(std::string name, Linkage stor, std::shared_ptr<IR_Type> fullType);
    IRval createReg(std::shared_ptr<IR_Type> type);
    IRval createGlobal(std::string name, std::shared_ptr<IR_Type> type, IRval init);

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
    int blocksCounter = 0;
    uint64_t regs_counter = 0;
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

#endif /* IR_UNIT_HPP_INCLUDED__ */
