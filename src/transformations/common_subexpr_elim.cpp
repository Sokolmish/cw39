#include "computing_transformers.hpp"
#include <map>

CommonSubexprElim::CommonSubexprElim(CFGraph rawCfg) : IRTransformer(std::move(rawCfg)) {
    std::map<std::pair<std::vector<IRval>, IR_ExprOper::IR_Ops>, IRval> operExprs;

    auto visitor = [this, &operExprs](int blockId) {
        IR_Block &block = cfg.block(blockId);
        for (IR_Node &node : block.body) {
            if (node.body->type == IR_Expr::OPERATION) {
                auto expr = dynamic_cast<IR_ExprOper const &>(*node.body);
                auto it = operExprs.find({ expr.args, expr.op });
                if (it == operExprs.end()) {
                    operExprs.emplace(std::make_pair(expr.args, expr.op), node.res.value());
                }
                else { // Found
                    node.body = std::make_unique<IR_ExprOper>(
                            IR_ExprOper::MOV, std::vector<IRval>{ it->second });
                }
            }
        }
    };

    std::set<int> visited;
    cfg.traverseBlocks(cfg.entryBlockId, visited, visitor);
}
