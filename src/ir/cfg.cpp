#include "cfg.hpp"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <sstream>
#include <stack>

ControlFlowGraph::Function ControlFlowGraph::Function::clone() const {
    auto res = ControlFlowGraph::Function();
    res.id = id;
    res.name = name;
    res.entryBlockId = entryBlockId;
    res.storage = storage;
    res.isInline = isInline;
    res.fullType = fullType->copy();
    return res;
}

int ControlFlowGraph::Function::getId() const {
    return id;
}

std::string ControlFlowGraph::Function::getName() const {
    return name;
}

int ControlFlowGraph::Function::getEntryBlockId() const {
    return entryBlockId;
}

std::shared_ptr<IR_TypeFunc> ControlFlowGraph::Function::getFuncType() const {
    return std::dynamic_pointer_cast<IR_TypeFunc>(fullType);
}


ControlFlowGraph::ControlFlowGraph(const ControlFlowGraph &oth) {
    blocksCounter = oth.blocksCounter;
    regs_counter = oth.regs_counter;
    funcsCounter = oth.funcsCounter;
    stringsCounter = oth.stringsCounter;
    globalsCounter = oth.globalsCounter;

    for (const auto &[id, block]: oth.blocks)
        blocks.insert(blocks.end(), { id, block.copy() });
    for (const auto &[id, func]: oth.funcs)
        funcs.emplace_hint(funcs.end(), id, func.clone());
    for (const auto &[id, func]: oth.prototypes)
        prototypes.emplace_hint(prototypes.end(), id, func.clone());
    for (const auto &[id, irStruct]: oth.structs)
        structs.emplace_hint(structs.end(), id, irStruct);
    strings = oth.strings;
    for (const auto &[id, global]: oth.globals) {
        globals.emplace_hint(globals.end(), id, GlobalVar{
                global.id, global.name, global.type->copy(), global.init.copy() });
    }
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

ControlFlowGraph::Function& ControlFlowGraph::getFunction(int id) {
    auto fIt = funcs.find(id);
    if (fIt != funcs.end())
        return fIt->second;
    auto pIt = prototypes.find(id);
    if (pIt != prototypes.end())
        return pIt->second;
    throw std::out_of_range("No functions with given id");
}

ControlFlowGraph::Function const& ControlFlowGraph::getFunction(int id) const {
    auto fIt = funcs.find(id);
    if (fIt != funcs.end())
        return fIt->second;
    auto pIt = prototypes.find(id);
    if (pIt != prototypes.end())
        return pIt->second;
    throw std::out_of_range("No functions with given id");
}

IRval ControlFlowGraph::createReg(std::shared_ptr<IR_Type> type) {
    return IRval::createReg(type, regs_counter++);
}

/** Expects ptr type */
IRval ControlFlowGraph::createGlobal(std::string name, std::shared_ptr<IR_Type> type, IRval init) {
    auto newId = globalsCounter++;
    IRval newGlobal = IRval::createGlobal(type, newId);
    globals.insert({ newId, GlobalVar{ newId, name, type, init } });
    return newGlobal;
}

ControlFlowGraph::Function& ControlFlowGraph::createFunction(std::string name,
        IR_StorageSpecifier stor, bool isInline, std::shared_ptr<IR_Type> fullType) {

    auto &newBlock = createBlock();
    Function func;
    func.id = funcsCounter++;
    func.name = std::move(name);
    func.entryBlockId = newBlock.id;
    func.storage = stor;
    func.isInline = isInline;
    func.fullType = fullType;
    auto it = funcs.emplace_hint(funcs.end(), func.id, std::move(func));
    return it->second;
}

ControlFlowGraph::Function& ControlFlowGraph::createPrototype(std::string name,
        IR_StorageSpecifier stor, std::shared_ptr<IR_Type> fullType) {
    Function func;
    func.id = funcsCounter++;
    func.name = std::move(name);
    func.entryBlockId = -1;
    func.storage = stor;
    func.isInline = false;
    func.fullType = fullType;
    auto it = prototypes.emplace_hint(prototypes.end(), func.id, std::move(func));
    return it->second;
}

uint64_t ControlFlowGraph::putString(std::string str) {
    uint64_t newStringId = stringsCounter++;
    strings.insert(strings.end(), { newStringId, std::move(str) });
    return newStringId;
}

const std::map<int, ControlFlowGraph::Function> &ControlFlowGraph::getFuncs() const {
    return funcs;
}

const std::map<int, ControlFlowGraph::Function> &ControlFlowGraph::getPrototypes() const {
    return prototypes;
}

std::map<int, IR_Block> const& ControlFlowGraph::getBlocks() const {
    return blocks;
}

const std::map<int, ControlFlowGraph::GlobalVar>& ControlFlowGraph::getGlobals() const {
    return globals;
}

std::map<string_id_t, std::shared_ptr<IR_TypeStruct>> const& ControlFlowGraph::getStructs() const {
    return structs;
}

std::map<uint64_t, std::string> const& ControlFlowGraph::getStrings() const {
    return strings;
}

void ControlFlowGraph::traverseBlocks(int blockId, std::set<int> &visited, std::function<void(int)> action) {
    if (visited.contains(blockId))
        return;
    visited.insert(blockId);
    action(blockId);
    for (int nextId : this->block(blockId).next)
        if (!visited.contains(nextId))
            traverseBlocks(nextId, visited, action);
}


/*
 * Printers
 */

void ControlFlowGraph::printExpr(std::stringstream &ss, const IR_Expr &rawExpr) const {
    if (rawExpr.type == IR_Expr::OPERATION) {
        ss << dynamic_cast<IR_ExprOper const &>(rawExpr).to_string();
    }
    else if (rawExpr.type == IR_Expr::ALLOCATION) {
        ss << dynamic_cast<IR_ExprAlloc const &>(rawExpr).to_string();
    }
    else if (rawExpr.type == IR_Expr::MEMORY) {
        ss << dynamic_cast<IR_ExprMem const &>(rawExpr).to_string();
    }
    else if (rawExpr.type == IR_Expr::CAST) {
        ss << dynamic_cast<IR_ExprCast const &>(rawExpr).to_string();
    }
    else if (rawExpr.type == IR_Expr::CALL) {
        auto const &expr = dynamic_cast<IR_ExprCall const &>(rawExpr);
        if (expr.isIndirect())
            fmt::print(ss, "call {} ( ", expr.getFuncPtr().to_string());
        else
            fmt::print(ss, "call {} ( ", getFunction(expr.getFuncId()).name);
        for (auto const &arg : expr.args)
            fmt::print(ss, "{} ", arg.to_string());
        fmt::print(ss, ")");
    }
    else { // TODO: PHI and term
        throw;
    }
}

void ControlFlowGraph::printBlock(std::stringstream &ss, IR_Block const &block) const {
    // Block header and previous blocks
    fmt::print(ss, "Block {}:", block.id);
    if (!block.prev.empty()) {
        fmt::print(ss, " ; prevs:", block.id);
        for (int prevId : block.prev)
            fmt::print(ss, " {}", prevId);
    }
    fmt::print(ss, "\n");

    // PHI nodes
    for (auto const &[phiRes, phiFunc] : block.phis) {
        if (!phiFunc) {
            fmt::print(ss, "nop\n");
            continue;
        }

        if (phiRes)
            fmt::print(ss, "{} <- ", phiRes->to_string());
        fmt::print(ss, "phi( ");
        auto const &phiExpr = dynamic_cast<IR_ExprPhi const &>(*phiFunc);
        for (auto const &[index, val] : phiExpr.args)
            fmt::print(ss, "{}[{}] ", val.to_string(), index);
        fmt::print(ss, ")\n");
    }

    // Instructions
    for (auto const &node : block.body) {
        if (!node.body) {
            fmt::print(ss, "nop\n");
            continue;
        }
        if (node.res.has_value())
            fmt::print(ss, "{} <- ", node.res->to_string());
        printExpr(ss, *node.body);
        ss << "\n";
    }

    // TODO: move this to terminator and use getAllNodes
    // Terminator
    if (!block.termNode) {
        fmt::print(ss, "; No terminator\n");
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::RET) {
        if (!block.getTerminator()->arg.has_value())
            fmt::print(ss, "ret\n");
        else
            fmt::print(ss, "ret {}\n", block.getTerminator()->arg->to_string());
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::JUMP) {
        fmt::print(ss, "jump ->{}\n", block.next[0]);
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::BRANCH) {
        fmt::print(ss, "branch {} ->{} ->{}\n", block.getTerminator()->arg->to_string(),
                   block.next[0], block.next[1]);
    }
    else {
        fmt::print(ss, "; Unknown terminator\n");
    }

    fmt::print(ss, "\n");
}

static void writeEscapified(std::stringstream &ss, std::string const &str) {
    static const std::map<char, std::string> escapes{
            { '\\', "\\\\" },
            { '\a', "\\a" },
            { '\b', "\\b" },
            { '\f', "\\f" },
            { '\n', "\\n" },
            { '\r', "\\r" },
            { '\t', "\\t" },
            { '\v', "\\v" },
    };

    for (char ch : str) {
        auto it = escapes.find(ch);
        if (it != escapes.end())
            ss << it->second;
        else
            ss << ch;
    }
}

std::string ControlFlowGraph::printIR() const {
    std::stringstream ss;
    for (auto const &[id, structDecl] : structs) {
        fmt::print(ss, "struct({}) {{ ", structDecl->structId);
        for (const auto &field : structDecl->fields)
            fmt::print(ss, "{} ", field.irType->to_string());
        fmt::print(ss, "}}\n");
    }
    fmt::print(ss, "\n");

    for (auto const &[id, str] : strings) {
        fmt::print(ss, "string_{} = \"", id);
        writeEscapified(ss, str);
        ss << "\"\n";
    }
    fmt::print(ss, "\n");

    for (auto const &[id, global] : globals) {
        fmt::print(ss, "{} @{} = {}; \"{}\"\n", global.type->to_string(),
                   global.id, global.init.to_string(), global.name);
    }
    fmt::print(ss, "\n");

    for (auto const &[id, func] : prototypes) {
        fmt::print(ss, "func {} {};\n", func.fullType->to_string(), func.name);
    }
    fmt::print(ss, "\n");

    for (auto const &[id, func] : funcs) {
        fmt::print(ss, "func {} {} -> block {}\n", func.name,
                   func.fullType->to_string(), func.entryBlockId);
    }
    fmt::print(ss, "\n");

    for (auto const &[id, block] : blocks) {
        printBlock(ss, block);
    }

    return ss.str();
}

std::string ControlFlowGraph::drawCFG() const {
    std::stringstream ss;

    fmt::print(ss, "digraph{{\n"
                   "node [fontname=\"helvetica\"]\n");
    for (auto const &[fId, func] : funcs) {
        std::set<int> visited;
        std::stack<int> stack;
        stack.push(func.entryBlockId);
        visited.insert(func.entryBlockId);

        while (!stack.empty()) {
            IR_Block const &curBlock = this->blocks.at(stack.top());
            stack.pop();
            drawBlock(ss, curBlock);
            for (int ind = 0; int nextId : curBlock.next) {
                fmt::print(ss, "block_{} -> block_{}", curBlock.id, nextId);
//                if (curBlock.next.size() == 2) {
//                    fmt::print(ss, "[dir=both, arrowtail={}]",
//                               ind == 0 ? "noneodot" : "noneobox");
//                    fmt::print(ss, "[label={}]",
//                               ind == 0 ? "true" : "false");
//                }
                ss << "\n";
                if (!visited.contains(nextId)) {
                    visited.insert(nextId);
                    stack.push(nextId);
                }
                ind++;
            }
            fmt::print(ss, "\n");
        }
    }
    fmt::print(ss, "}}\n");
    return ss.str();
}

void ControlFlowGraph::drawBlock(std::stringstream &ss, IR_Block const &block) const {
    using namespace std::string_literals;
    static const std::string rightArrow = "\u2192"s; // ->
    static const std::string leftArrow = "\u2190"s;  // <-
    static const std::string phiSign = "\u03d5"s;

    std::stringstream ssb;

    fmt::print(ssb, "block_{}:\\l", block.id);
    fmt::print(ssb, "--------------------\\l");

    // PHIs
    for (auto const &[phiRes, phiFunc]: block.phis) {
        if (!phiFunc) {
            fmt::print(ssb, "nop\\l");
            continue;
        }
        if (phiRes)
            fmt::print(ssb, "{} {} ", phiRes->to_string(), leftArrow);
        fmt::print(ssb, "{}( ", phiSign);
        auto const &phiExpr = dynamic_cast<IR_ExprPhi const &>(*phiFunc);
        for (auto const &[index, val]: phiExpr.args)
            fmt::print(ssb, "{}[ b{} ] ", val.to_string(), block.prev.at(index));
        fmt::print(ssb, ")\\l");
    }

    // Instructions
    for (auto const &node: block.body) {
        if (!node.body) {
            fmt::print(ssb, "nop\\l");
            continue;
        }
        if (node.res.has_value())
            fmt::print(ssb, "{} {} ", node.res->to_string(), leftArrow);
        printExpr(ssb, *node.body);
        ssb << "\\l";
    }

    // Terminator
    if (!block.termNode) {
        fmt::print(ssb, "; No terminator\\l");
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::RET) {
        if (!block.getTerminator()->arg.has_value())
            fmt::print(ssb, "ret\\l");
        else
            fmt::print(ssb, "ret {}\\l", block.getTerminator()->arg->to_string());
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::JUMP) {
        fmt::print(ssb, "jump {}b{}\\l", rightArrow, block.next[0]);
    }
    else if (block.getTerminator()->termType == IR_ExprTerminator::BRANCH) {
        fmt::print(ssb, "branch {} true{}b{} false{}b{}\\l",
                   block.getTerminator()->arg->to_string(),
                   rightArrow, block.next[0], rightArrow, block.next[1]);
    }
    else {
        fmt::print(ssb, "; Unknown terminator\\l");
    }

    fmt::print(ss, "block_{} [shape=rectangle,label=\"{}\"]\n", block.id, ssb.str());
}
