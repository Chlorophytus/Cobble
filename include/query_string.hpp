#if !defined(COBBLE_QUERY_STRING)
#define COBBLE_QUERY_STRING
#include "main.hpp"
#include <string>
#include <unordered_map>
namespace cobble {
/// @brief Handles HTTP query strings
namespace query_string {
/// @brief Parses a HTTP query string
/// @param what What to parse
/// @param path The string to fill with the query string's path
/// @return a map with keys of the query string mapping to their values
std::unordered_map<std::string, std::string> parse(const std::string &what, std::string &path);
} // namespace query_string
} // namespace cobble
#endif