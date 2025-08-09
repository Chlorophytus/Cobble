#include "../include/environment.hpp"
#include "../include/logger.hpp"
#include <exception>
#include <stdexcept>
#include <utility>
using namespace cobble;

environment::configuration
environment::load(const std::filesystem::path &where) {
  configuration config{};
  logger::log(logger::severity::informational, "Loading configuration '", where,
              "'");
  toml::table table;

  table = toml::parse_file(where.string());

  config.listen_address = boost::asio::ip::make_address(
      table["http"]["listen"].value<std::string>()->c_str());

  S64 listen_port_candidate = *table["http"]["port"].value<S64>();
  if (!std::in_range<U16>(listen_port_candidate)) {
    throw std::runtime_error{"Listener port must be 0-65535"};
  }
  config.listen_port = listen_port_candidate;

  S64 threads_candidate = *table["http"]["threads"].value<S64>();
  if(threads_candidate < 1) {
    throw std::runtime_error{"I/O context threads count must be above zero"};
  }
  config.threads = threads_candidate;

  return config;
}