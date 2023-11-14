#ifndef CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H
#define CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H

#include "Transformation.hpp"

namespace clp_structured {
// Constant propagate empty expressions keeping all remaining data IN PLACE
class ConstantProp : public Transformation {
public:
    std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

private:
    static std::shared_ptr<Expression> propagate_empty(std::shared_ptr<Expression> cur);
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_CONSTANT_PROP_H
