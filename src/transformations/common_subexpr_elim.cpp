#include "computing_transformers.hpp"
#include <map>

CommonSubexprElim::CommonSubexprElim(IntermediateUnit const &unit, CFGraph rawCfg)
        : IRTransformer(std::move(rawCfg)) {
    std::map<std::pair<std::vector<IRval>, IR_ExprOper::IR_Ops>, IRval> operExprs;

    auto visitor = [this, &operExprs](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Node &node : block.body) {
            if (auto exprOper = node.body->toOper()) {
                auto it = operExprs.find({ exprOper->args, exprOper->op });
                if (it == operExprs.end()) {
                    operExprs.emplace(std::make_pair(exprOper->args, exprOper->op), node.res.value());
                }
                else { // Found
                    node.body = std::make_unique<IR_ExprOper>(
                            IR_ExprOper::MOV, std::vector<IRval>{ it->second });
                    setPassChanged();
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
}
