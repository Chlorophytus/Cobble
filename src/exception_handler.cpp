#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include <typeinfo>
#include <exception>
using namespace cobble;

void exception_handler::print_nested(const std::exception &e, U32 level) {
  logger::log(logger::severity::error, "[", level,
              "] ", typeid(e).name(), ": ", e.what());

  try {
    std::rethrow_if_nested(e);
  } catch (const std::exception &nested) {
    print_nested(nested, level + 1);
  }
}