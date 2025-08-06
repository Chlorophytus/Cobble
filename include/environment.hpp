#if !defined(COBBLE_ENVIRONMENT)
#define COBBLE_ENVIRONMENT
#include "main.hpp"
#include <filesystem>
#include <toml++/toml.hpp>
namespace cobble {
/// @brief Handles configuration via a TOML file
namespace environment {
/// @brief A configuration structure
struct configuration {};

/// @brief Load a TOML configuration, throws an error if invalid
/// @param where The file path
/// @return A configuration
configuration load(const std::filesystem::path &where);
} // namespace environment
} // namespace cobble
#endif