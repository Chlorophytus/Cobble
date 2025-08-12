#if !defined(COBBLE_ENVIRONMENT)
#define COBBLE_ENVIRONMENT
#include "main.hpp"
#include <boost/asio.hpp>
#include <filesystem>
#include <forward_list>
#include <string>
#include <toml++/toml.hpp>
#include <variant>
namespace cobble {
/// @brief Handles configuration via a TOML file
namespace environment {
/// @brief Contains lists of both IPv4 and IPv6 subnet data
struct cidr_network_list {
  /// @brief IPv4 nets
  std::forward_list<boost::asio::ip::network_v4> v4{};

  /// @brief IPv6 nets
  std::forward_list<boost::asio::ip::network_v6> v6{};
};

/// @brief A configuration structure
struct configuration {
  /// @brief The IP address we listen with
  boost::asio::ip::address listen_address;

  /// @brief The port we listen on
  U16 listen_port;

  /// @brief How many threads the server I/O context uses
  S32 threads;

  /// @brief Allowed CORS origin IPv6 ranges OR allowed CORS origin URLs
  std::variant<std::forward_list<std::string>, cidr_network_list> cors_entries;
};

/// @brief Load a TOML configuration, throws an error if invalid
/// @param where The file path
/// @return A configuration
configuration load(const std::filesystem::path &where);
} // namespace environment
} // namespace cobble
#endif