#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include <typeinfo>
#include <exception>
#include <cxxabi.h>
using namespace cobble;

void exception_handler::print_nested(const std::exception &e, U32 level) {
  int status;
  char *name;

  // demangle name with C++ ABI helper, need to free it afterward
  // https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
  name = abi::__cxa_demangle(typeid(e).name(), nullptr, nullptr, &status);
  logger::log(logger::severity::error, "[", level,
              "] ", name, ": ", e.what());
  std::free(name);

  try {
    std::rethrow_if_nested(e);
  } catch (const std::exception &nested) {
    print_nested(nested, level + 1);
  }
}