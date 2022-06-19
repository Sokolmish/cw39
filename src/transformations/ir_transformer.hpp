#ifndef IR_TRANSFORMER_HPP_INCLUDED__
#define IR_TRANSFORMER_HPP_INCLUDED__

#include "ir/unit.hpp"

class IRTransformer {
public:
    IRTransformer(CFGraph cfg) : cfg(std::move(cfg)) {}

    CFGraph const& getCfg() const {
        return cfg;
    };

    CFGraph moveCfg() && {
        return std::move(cfg);
    };

    bool isPassEffective() const {
        return pass_changed;
    }

protected:
    CFGraph cfg;

    void setPassChanged() {
        pass_changed = true;
    }

private:
    bool pass_changed = false;
};

#endif /* IR_TRANSFORMER_HPP_INCLUDED__ */
