#if !defined(COBBLE_ENVIRONMENT)
#define COBBLE_ENVIRONMENT
#include "main.hpp"
#include <boost/asio.hpp>
#include <filesystem>
#include <toml++/toml.hpp>
namespace cobble {
/// @brief Handles configuration via a TOML file
namespace environment {
/// @brief A configuration structure
struct configuration {
  /// @brief The IP address we listen with
  boost::asio::ip::address listen_address;

  /// @brief The port we listen on
  U16 listen_port;

  /// @brief How many threads the server I/O context uses
  S32 threads;
};

/// @brief Load a TOML configuration, throws an error if invalid
/// @param where The file path
/// @return A configuration
configuration load(const std::filesystem::path &where);
} // namespace environment
} // namespace cobble
#endif