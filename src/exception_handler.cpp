#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include <format>
#include <typeinfo>
using namespace cobble;

void exception_handler::print_nested(const std::exception &e, U32 level) {
  logger::log(logger::severity::error, "[",
              std::format("0x{:016x}", reinterpret_cast<U64>(&e)), " @ ", level,
              "] ", typeid(e).name(), ": ", e.what());

  try {
    std::rethrow_if_nested(e);
  } catch (const std::exception &nested) {
    print_nested(e, level + 1);
  }
}