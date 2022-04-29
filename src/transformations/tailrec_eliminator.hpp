#ifndef TAILREC_ELIMINATOR_HPP_INCLUDED__
#define TAILREC_ELIMINATOR_HPP_INCLUDED__

#include <vector>
#include "ir_transformer.hpp"

class TailrecEliminator : public IRTransformer {
public:
    explicit TailrecEliminator(CFGraph rawCfg, int funcId);

private:
    void passFunction(int funcId);
    std::vector<int> findTailCalls(int funcId);
    void replaceParams(int entryId, const std::vector<IRval> &newArgs);
};

#endif /* TAILREC_ELIMINATOR_HPP_INCLUDED__ */
