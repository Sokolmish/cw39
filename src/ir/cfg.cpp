#include "cfg.hpp"
#include <fmt/core.h>
#include <sstream>

ControlFlowGraph::Function ControlFlowGraph::Function::clone() const {
    auto res = ControlFlowGraph::Function();
    res.id = id;
    res.entryBlockId = entryBlockId;
    res.storage = storage;
    res.isInline = isInline;
    res.fullType = fullType->copy();
    return res;
}

int ControlFlowGraph::Function::getId() const {
    return id;
}

int ControlFlowGraph::Function::getEntryBlockId() const {
    return entryBlockId;
}

IR_TypeFunc const& ControlFlowGraph::Function::getFuncType() const {
    return dynamic_cast<IR_TypeFunc const&>(*fullType);
}


ControlFlowGraph::ControlFlowGraph(const ControlFlowGraph &oth) {
    blocksCounter = oth.blocksCounter;
    regs_counter = oth.blocksCounter;
    funcsCounter = oth.funcsCounter;

    for (const auto &[id, block] : oth.blocks)
        blocks.insert(blocks.end(), { id, block.copy() });
    for (const auto &[id, block] : oth.funcs)
        funcs.emplace_hint(funcs.end(), id, block);
}

IR_Block &ControlFlowGraph::createBlock() {
    int newId = blocksCounter++;
    auto it = blocks.emplace_hint(blocks.end(), newId, IR_Block(newId));
    return it->second;
}

void ControlFlowGraph::linkBlocks(IR_Block &prev, IR_Block &next) {
    prev.next.push_back(next.id);
    next.prev.push_back(prev.id);
}

IR_Block &ControlFlowGraph::block(int id) {
    return blocks.at(id);
}

ControlFlowGraph::Function &ControlFlowGraph::getFunction(int id) {
    return funcs.at(id);
}

IRval ControlFlowGraph::createReg(std::shared_ptr<IR_Type> type) {
    return IRval::createReg(type, regs_counter++);
}

ControlFlowGraph::Function& ControlFlowGraph::createFunction(
        IR_StorageSpecifier stor, bool isInline, std::shared_ptr<IR_Type> fullType) {

    auto &newBlock = createBlock();
    Function func;
    func.id = funcsCounter++;
    func.entryBlockId = newBlock.id;
    func.storage = stor;
    func.isInline = isInline;
    func.fullType = fullType;
    auto it = funcs.emplace_hint(funcs.end(), func.id, std::move(func));
    return it->second;
}


/*
 * Printers
 */

static std::string printType(IR_Type const &type) {
    if (type.type == IR_Type::POINTER) {
        auto const &ptr = dynamic_cast<IR_TypePtr const &>(type);
        return fmt::format("ptr< {} >", printType(*ptr.child));
    }
    else if (type.type == IR_Type::ARRAY) {
        auto const &arr = dynamic_cast<IR_TypeArray const &>(type);
        return fmt::format("array< {} * {} >", arr.size, printType(*arr.child));
    }
    else if (type.type == IR_Type::FUNCTION) {
        std::stringstream ss;
        auto const &fun = dynamic_cast<IR_TypeFunc const &>(type);
        ss << "fun< " << printType(*fun.ret) << " : ";
        for (auto const &arg : fun.args)
            ss << printType(*arg) << " ";
        if (fun.isVariadic)
            ss << "... ";
        ss << ">";
        return ss.str();
    }
    else if (type.type == IR_Type::DIRECT) {
        auto const &dir = dynamic_cast<IR_TypeDirect const &>(type);
        switch (dir.spec) {
            case IR_TypeDirect::U8:
                return "u8";
            case IR_TypeDirect::I8:
                return "i8";
            case IR_TypeDirect::U32:
                return "u32";
            case IR_TypeDirect::I32:
                return "i32";
            case IR_TypeDirect::U64:
                return "u64";
            case IR_TypeDirect::I64:
                return "i64";
            case IR_TypeDirect::F32:
                return "f32";
            case IR_TypeDirect::VOID:
                return "void";
        }
    }
    throw;
}

static void printBlock(IR_Block const &block) {
    fmt::print("Block {}:\n", block.id);
    for (auto const &node: block.body) {
        if (node.res.has_value())
            fmt::print("{} <- ", node.res->to_string());
        if (node.body->type == IR_Expr::OPERATION) {
            auto const &expr = dynamic_cast<IR_ExprOper const &>(*node.body);
            fmt::print("{}", expr.opToString());
            for (auto const &arg: expr.args)
                fmt::print(" {}", arg.to_string());
            fmt::print("\n");
        }
        else if (node.body->type == IR_Expr::ALLOCATION) {
            auto const &expr = dynamic_cast<IR_ExprAlloc const &>(*node.body);
            fmt::print("{} {} x {}\n", expr.opToString(), printType(*expr.type), expr.size);
        }
        else if (node.body->type == IR_Expr::CAST) {
            auto const &expr = dynamic_cast<IR_ExprCast const &>(*node.body);
            fmt::print("{} {} : {} -> {}\n", expr.opToString(), expr.arg.to_string(),
                       printType(*expr.arg.getType()), printType(*expr.dest));
        }
        else if (node.body->type == IR_Expr::CALL) {
            auto const &expr = dynamic_cast<IR_ExprCall const &>(*node.body);
            fmt::print("call {} ( ", expr.funcId);
            for (auto const &arg : expr.args)
                fmt::print("{} ", arg.to_string());
            fmt::print(")\n");
        }
    }

    if (block.terminator.type == IR_Terminator::NONE) {
        fmt::print("; No terminator\n");
    }
    else if (block.terminator.type == IR_Terminator::RET) {
        if (!block.terminator.arg.has_value())
            fmt::print("ret\n");
        else
            fmt::print("ret {}\n", block.terminator.arg->to_string());
    }
    else if (block.terminator.type == IR_Terminator::JUMP) {
        fmt::print("jump ->{}\n", block.next[0]);
    }
    else if (block.terminator.type == IR_Terminator::BRANCH) {
        fmt::print("branch {} ->{} ->{}\n", block.terminator.arg->to_string(),
                   block.next[0], block.next[1]);
    }
    else {
        fmt::print("; Unknown terminator\n");
    }

    fmt::print("\n");
}

void ControlFlowGraph::printBlocks() const {
    for (auto const &[id, func] : funcs)
        fmt::print("func {} -> block {}\n", func.id, func.entryBlockId);
    fmt::print("\n");
    for (auto const &[id, block] : blocks) {
        printBlock(block);
    }
}
