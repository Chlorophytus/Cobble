#include "../include/environment.hpp"
#include "../include/logger.hpp"
#include <exception>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace cobble;

environment::configuration
environment::load(const std::filesystem::path &where) {
  configuration config{};
  logger::log(logger::severity::informational, "Loading configuration '", where,
              "'");
  toml::table table;

  table = toml::parse_file(where.string());

  config.data_path = std::filesystem::path{
      *table["storage"]["directory"].value<std::string>()};

  config.listen_address = boost::asio::ip::make_address(
      table["http"]["listen"].value<std::string>()->c_str());

  S64 listen_port_candidate = *table["http"]["port"].value<S64>();
  if (!std::in_range<U16>(listen_port_candidate)) {
    throw std::runtime_error{"Listener port must be 0-65535"};
  }
  config.listen_port = listen_port_candidate;

  S64 threads_candidate = *table["http"]["threads"].value<S64>();
  if (threads_candidate < 1) {
    throw std::runtime_error{"I/O context threads count must be above zero"};
  }
  config.threads = threads_candidate;

  if (table["http"]["cors"]["force_cidr"].value_or<bool>(false)) {
    config.cors_entries = cidr_network_list{};

    auto cors_array4 = table["http"]["cors"]["origins"]["v4"].as_array();
    toml::array cors_array_entries4 = *cors_array4;

    auto cors_array6 = table["http"]["cors"]["origins"]["v6"].as_array();
    toml::array cors_array_entries6 = *cors_array6;

    auto &&entries = std::get<cidr_network_list>(config.cors_entries);
    for (auto i = 0; i < cors_array_entries4.size(); i++) {
      auto v4 = cors_array_entries4[i].value<std::string>();
      if (v4) {
        logger::log(logger::severity::debug,
                    "Adding IPv4 subnet to CORS list: '", *v4, "'");
        entries.v4.emplace_front(boost::asio::ip::make_network_v4(*v4));
      } else {
        throw std::runtime_error{
            "One of your CORS origins were not a string type."};
      }
    }

    for (auto i = 0; i < cors_array_entries6.size(); i++) {
      auto v6 = cors_array_entries4[i].value<std::string>();
      if (v6) {
        logger::log(logger::severity::debug,
                    "Adding IPv6 subnet to CORS list: '", *v6, "'");
        entries.v6.emplace_front(boost::asio::ip::make_network_v6(*v6));
      } else {
        throw std::runtime_error{
            "One of your CORS origins were not a string type."};
      }
    }
  } else {
    config.cors_entries = std::forward_list<std::string>{};

    auto cors_array = table["http"]["cors"]["origins"].as_array();
    toml::array cors_array_entries = *cors_array;

    auto &&entries =
        std::get<std::forward_list<std::string>>(config.cors_entries);

    for (auto i = 0; i < cors_array_entries.size(); i++) {
      auto domain = cors_array_entries[i].value<std::string>();
      if (domain) {
        logger::log(logger::severity::debug, "Adding DNS domain CORS list: '",
                    *domain, "'");
        entries.emplace_front(*domain);
      } else {
        throw std::runtime_error{
            "One of your CORS origins were not a string type."};
      }
    }
  }

  return config;
}