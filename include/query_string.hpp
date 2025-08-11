#if !defined(COBBLE_QUERY_STRING)
#define COBBLE_QUERY_STRING
#include "main.hpp"
#include <string>
#include <filesystem>
#include <unordered_map>
namespace cobble {
/// @brief Handles HTTP query strings
namespace query_string {
/// @brief Parses a HTTP query string
/// @param what What to parse
/// @param path The query string's path
/// @return a map with keys of the query string mapping to their values
std::unordered_map<std::string, std::string> parse(const std::string &what,
                                                   std::filesystem::path &path);
} // namespace query_string
} // namespace cobble
#endif