#include "../include/main.hpp"
#include "../include/environment.hpp"
#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include <cstdlib>
#include <exception>
using namespace cobble;

int main(int argc, char **argv) {
  try {
    logger::all_loggers().emplace_back(new logger::stdout_listener());
    logger::all_loggers().emplace_back(
        new logger::file_listener("console.log"));
    logger::log(logger::severity::notice, "Cobble v", Cobble_VSTRING_FULL);

    if(argc < 2) {
      throw std::runtime_error{"Need to specify a TOML configuration file"};
    }

    environment::configuration config = environment::load(argv[1]);

    return EXIT_SUCCESS;
  } catch (const std::exception &e) {
    logger::log(logger::severity::emergency, "/!\\ Terminating, stacktrace is below... /!\\");
    exception_handler::print_nested(e);

    return EXIT_FAILURE;
  }
}