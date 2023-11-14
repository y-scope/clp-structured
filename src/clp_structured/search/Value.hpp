#ifndef CLP_STRUCTURED_SEARCH_VALUE_H
#define CLP_STRUCTURED_SEARCH_VALUE_H

#include <iostream>

namespace clp_structured {
/**
 * Class representing a generic value in the AST. Key subclasses are Literal and Expression.
 */
class Value {
public:
    virtual unsigned get_num_operands() = 0;

    /**
     * Print a string representation of the value to standard error.
     *
     * Useful for debugging in gdb.
     */
    virtual void print() = 0;

    virtual ~Value() = default;

protected:
    static std::ostream& get_print_stream() { return std::cerr; }
};
}  // namespace clp_structured

#endif
