#include "../include/main.hpp"
#include "../include/environment.hpp"
#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include "../include/server.hpp"
#include <csignal>
#include <cstdlib>
#include <exception>
#include <stdexcept>
using namespace cobble;
std::atomic<bool> run;

void signal_handler(int signal) { run = false; }

int main(int argc, char **argv) {
  try {
    logger::all_loggers().emplace_back(new logger::stdout_listener());
    logger::all_loggers().emplace_back(
        new logger::file_listener("console.log"));
    logger::log(logger::severity::notice, "Cobble v", Cobble_VSTRING_FULL);

    if (argc < 2) {
      throw std::runtime_error{"Please pass a TOML configuration file"};
    }

    run = true;
    environment::configuration config = environment::load(argv[1]);

    logger::log(
        logger::severity::notice,
        "Hooking SIGINT, press Ctrl-C to gracefully shut down the server");

    std::signal(SIGINT, signal_handler);
    server::start(config, run);

    logger::log(logger::severity::notice, "Server shut down gracefully");
    return EXIT_SUCCESS;
  } catch (const std::exception &e) {
    logger::log(logger::severity::emergency,
                "Terminating, stacktrace is below");
    exception_handler::print_nested(e);

    return EXIT_FAILURE;
  }
}