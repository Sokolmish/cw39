#include "unit.hpp"
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <sstream>
#include <stack>

IntermediateUnit::Function::Function(int id, std::string name, int isProto)
        : cfg(), isProto(isProto), id(id), name(std::move(name)) {}

IntermediateUnit::Function::Function(const Function &oth)
        : storage(oth.storage), fullType(oth.fullType->copy()), fspec(oth.fspec),
          cfg(oth.cfg.copy()), isProto(oth.isProto), id(oth.id), name(oth.name) {}


int IntermediateUnit::Function::getId() const {
    return id;
}

std::string IntermediateUnit::Function::getName() const {
    return name;
}

std::shared_ptr<IR_TypeFunc> IntermediateUnit::Function::getFuncType() const {
    return std::dynamic_pointer_cast<IR_TypeFunc>(fullType);
}

bool IntermediateUnit::Function::isPure() const {
    return fspec & FSPEC_PURE;
}

bool IntermediateUnit::Function::isInline() const {
    return fspec & FSPEC_INLINE;
}


IntermediateUnit::IntermediateUnit(const IntermediateUnit &oth) {
    funcsCounter = oth.funcsCounter;
    stringsCounter = oth.stringsCounter;
    globalsCounter = oth.globalsCounter;

    for (const auto &[id, func]: oth.funcs)
        funcs.emplace_hint(funcs.end(), id, Function(func));
    for (const auto &[id, func]: oth.prototypes)
        prototypes.emplace_hint(prototypes.end(), id, Function(func));
    for (const auto &[id, irStruct]: oth.structs)
        structs.emplace_hint(structs.end(), id, irStruct);
    strings = oth.strings;
    for (const auto &[id, global]: oth.globals) {
        globals.emplace_hint(globals.end(), id, GlobalVar{
                global.id, global.name, global.type->copy(), global.init.copy(), global.storage });
    }
}

IntermediateUnit::Function& IntermediateUnit::getFunction(int id) {
    auto fIt = funcs.find(id);
    if (fIt != funcs.end())
        return fIt->second;
    auto pIt = prototypes.find(id);
    if (pIt != prototypes.end())
        return pIt->second;
    throw cw39_internal_error("No functions with given id");
}

IntermediateUnit::Function const& IntermediateUnit::getFunction(int id) const {
    auto fIt = funcs.find(id);
    if (fIt != funcs.end())
        return fIt->second;
    auto pIt = prototypes.find(id);
    if (pIt != prototypes.end())
        return pIt->second;
    throw cw39_internal_error("No functions with given id");
}

/** Expects ptr type */
IRval IntermediateUnit::createGlobal(std::string name, std::shared_ptr<IR_Type> type, IRval init, VarLinkage stor) {
    auto newId = globalsCounter++;
    IRval newGlobal = IRval::createGlobal(std::move(type), newId);
    globals.insert({ newId, GlobalVar{
            .id = newId,
            .name = std::move(name),
            .type = newGlobal.getType(),
            .init = std::move(init),
            .storage = stor,
    }});
    return newGlobal;
}

IntermediateUnit::Function& IntermediateUnit::createFunction(std::string name, FunLinkage stor, int fspec,
                                                             std::shared_ptr<IR_Type> fullType) {
    Function func(funcsCounter++, std::move(name), false);
    func.storage = stor;
    func.fspec = fspec;
    func.fullType = std::move(fullType);
    auto it = funcs.emplace_hint(funcs.end(), func.id, std::move(func));
    return it->second;
}

IntermediateUnit::Function& IntermediateUnit::createPrototype(std::string name, FunLinkage stor,
                                                              std::shared_ptr<IR_Type> fullType) {
    Function func(funcsCounter++, std::move(name), true);
    func.storage = stor;
    func.fspec = Function::FuncSpec::FSPEC_NONE;
    func.fullType = std::move(fullType);
    auto it = prototypes.emplace_hint(prototypes.end(), func.id, std::move(func));
    return it->second;
}

uint64_t IntermediateUnit::putString(std::string str) {
    uint64_t newStringId = stringsCounter++;
    strings.insert(strings.end(), { newStringId, std::move(str) });
    return newStringId;
}

const std::map<int, IntermediateUnit::Function> &IntermediateUnit::getFuncs() const {
    return funcs;
}

std::map<int, IntermediateUnit::Function> &IntermediateUnit::getFuncsMut() {
    return funcs;
}


const std::map<int, IntermediateUnit::Function> &IntermediateUnit::getPrototypes() const {
    return prototypes;
}

const std::map<int, IntermediateUnit::GlobalVar>& IntermediateUnit::getGlobals() const {
    return globals;
}

std::map<string_id_t, std::shared_ptr<IR_TypeStruct>> const& IntermediateUnit::getStructs() const {
    return structs;
}

std::map<uint64_t, std::string> const& IntermediateUnit::getStrings() const {
    return strings;
}

/*
 * Printers
 */

void IntermediateUnit::printExpr(std::stringstream &ss, const IR_Expr &rawExpr) const {
    if (auto exprOper = dynamic_cast<IR_ExprOper const *>(&rawExpr)) {
        ss << exprOper->to_string();
    }
    else if (auto exprMem = dynamic_cast<IR_ExprMem const *>(&rawExpr)) {
        ss << exprMem->to_string();
    }
    else if (auto exprAccess = dynamic_cast<IR_ExprAccess const *>(&rawExpr)) {
        ss << exprAccess->to_string();
    }
    else if (auto exprCast = dynamic_cast<IR_ExprCast const *>(&rawExpr)) {
        ss << exprCast->to_string();
    }
    else if (auto exprCall = dynamic_cast<IR_ExprCall const *>(&rawExpr)) {
        if (exprCall->isIndirect())
            fmt::print(ss, "call {} ( ", exprCall->getFuncPtr().to_string());
        else
            fmt::print(ss, "call {} ( ", getFunction(exprCall->getFuncId()).name);
        for (auto const &arg : exprCall->args)
            fmt::print(ss, "{} ", arg.to_string());
        fmt::print(ss, ")");
    }
    else if (auto exprAlloc = dynamic_cast<IR_ExprAlloc const *>(&rawExpr)) {
        ss << exprAlloc->to_string();
    }
    else { // TODO: PHI and term
        throw cw39_not_implemented("Printing phi-functions and terminators");
    }
}

void IntermediateUnit::printBlock(std::stringstream &ss, IR_Block const &block) const {
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

std::string IntermediateUnit::printIR() const {
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

    for (auto const &[fId, func] : funcs) {
        fmt::print(ss, "func {} {} -> {} {{\n\n", func.name,
                   func.fullType->to_string(), func.cfg.entryBlockId);
        for (auto const &[bId, block] : func.cfg.getBlocks()) {
            printBlock(ss, block);
        }
        fmt::print(ss, "}} ; {}\n\n", func.name);
    }

    return ss.str();
}

std::string IntermediateUnit::drawCFG() const {
    std::stringstream ss;

    fmt::print(ss, "digraph{{\n"
                   "node [fontname=\"helvetica\",shape=box]\n");

    for (auto const &[fId, func] : funcs) {
        std::set<int> visited;
        std::stack<int> stack;
        stack.push(func.cfg.entryBlockId);
        visited.insert(func.cfg.entryBlockId);

        while (!stack.empty()) {
            IR_Block const &curBlock = func.cfg.block(stack.top());
            stack.pop();
            drawBlock(ss, curBlock, func.id);
            for (int ind = 0; int nextId : curBlock.next) {
                fmt::print(ss, "bl{}_f{} -> bl{}_f{}",
                           curBlock.id, func.id, nextId, func.id);
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

void IntermediateUnit::drawBlock(std::stringstream &ss, IR_Block const &block, int funcId) const {
    using namespace std::string_literals;
    static const std::string rightArrow = "\u2192"s; // ->
    static const std::string leftArrow = "\u2190"s;  // <-
    static const std::string phiSign = "\u03d5"s;

    std::stringstream ssb;

    fmt::print(ssb, "block {}\\n\\n", block.id);

    // PHIs
    for (auto const &[phiRes, phiFunc]: block.phis) {
        if (!phiFunc) {
            fmt::print(ssb, "nop\\l");
            continue;
        }
        if (phiRes)
            fmt::print(ssb, "{} {} ", phiRes->to_string(), leftArrow);
        fmt::print(ssb, "{} ( ", phiSign);
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
    fmt::print(ss, "bl{}_f{} [label=\"{}\"]\n", block.id, funcId, ssb.str());
}


CFGraph CFGraph::copy() const {
    CFGraph res;
    res.entryBlockId = entryBlockId;
    for (const auto &[id, block]: blocks) {
//        res.blocks.emplace_hint(res.blocks.end(), id, block.copy());
        res.blocks.emplace(id, block.copy());
    }
    res.regsCounter = regsCounter;
    res.blocksCounter = blocksCounter;
    return res;
}

IR_Block &CFGraph::createBlock() {
    int newId = blocksCounter++;
    auto it = blocks.emplace_hint(blocks.end(), newId, IR_Block(newId));
    return it->second;
}

IR_Block &CFGraph::insertBlock(IR_Block block) {
    int newId = blocksCounter++;
    block.id = newId;
    auto it = blocks.emplace_hint(blocks.end(), newId, std::move(block));
    return it->second;
}

void CFGraph::linkBlocks(IR_Block &prev, IR_Block &next) {
    prev.next.push_back(next.id);
    next.prev.push_back(prev.id);
}

void CFGraph::linkBlocks(int prevId, int nextId) {
    block(prevId).next.push_back(nextId);
    block(nextId).prev.push_back(prevId);
}


IR_Block &CFGraph::block(int id) {
    return blocks.at(id);
}

IR_Block const& CFGraph::block(int id) const {
    return blocks.at(id);
}

std::map<int, IR_Block> const& CFGraph::getBlocks() const {
    return blocks;
}

std::map<int, IR_Block> &CFGraph::getBlocksData() {
    return blocks;
}

void CFGraph::traverseBlocks(int blockId, std::set<int> &visited,
                             std::function<void(int)> const &action) const {
    if (visited.contains(blockId))
        return;
    visited.insert(blockId);
    action(blockId);
    for (int nextId : this->block(blockId).next)
        if (!visited.contains(nextId))
            traverseBlocks(nextId, visited, action);
}

IRval CFGraph::createReg(std::shared_ptr<IR_Type> type) {
    return IRval::createReg(std::move(type), regsCounter++);
}
