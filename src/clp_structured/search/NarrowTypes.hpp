#ifndef CLP_STRUCTURED_SEARCH_NARROW_TYPES_H
#define CLP_STRUCTURED_SEARCH_NARROW_TYPES_H

#include "Transformation.hpp"

namespace clp_structured {
class NarrowTypes : public Transformation {
public:
    std::shared_ptr<Expression> run(std::shared_ptr<Expression>& expr) override;

private:
    static std::shared_ptr<Expression> narrow(std::shared_ptr<Expression> cur);
};
}  // namespace clp_structured

#endif  // CLP_STRUCTURED_SEARCH_NARROW_TYPES_H
