#ifndef TAILREC_ELIMINATOR_HPP_INCLUDED__
#define TAILREC_ELIMINATOR_HPP_INCLUDED__

#include <vector>
#include "ir_transformer.hpp"

class TailrecEliminator : IRTransformer {
public:
    explicit TailrecEliminator(CFGraph rawCfg, int funcId);

    CFGraph const& getCfg() override;
    CFGraph moveCfg() && override;

private:
    CFGraph cfg;

    void passFunction(int funcId);
    std::vector<int> findTailCalls(int funcId);
    void replaceParams(int entryId, const std::vector<IRval> &newArgs);
};

#endif /* TAILREC_ELIMINATOR_HPP_INCLUDED__ */
