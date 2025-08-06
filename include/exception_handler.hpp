#if !defined(COBBLE_EXCEPTION_HANDLER)
#define COBBLE_EXCEPTION_HANDLER
#include "main.hpp"
#include <exception>
namespace cobble {
/// @brief Handles C++ exception stack traces
namespace exception_handler {
/// @brief Recursively prints a nested stack trace
/// @param e The exception to trace
/// @param level Don't touch this, keep at 0
void print_nested(const std::exception &e, U32 level = 0);
} // namespace exception_handler
} // namespace cobble
#endif // COBBLE_EXCEPTION_HANDLER
