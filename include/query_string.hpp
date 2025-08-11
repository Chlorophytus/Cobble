#if !defined(COBBLE_QUERY_STRING)
#define COBBLE_QUERY_STRING
#include "main.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>
namespace cobble {
/// @brief Handles HTTP query strings
namespace query_string {
/// @brief Parses a HTTP query string and its path
/// @param what What to parse
/// @param path The query string's path
/// @return A map with keys of the query string mapping to their values
std::unordered_map<std::string, std::string> parse(const std::string &what,
                                                   std::filesystem::path &path);
/// @brief Parses only a HTTP query string (i.e. forms)
/// @param what What to parse
/// @return A map with keys of the query string mapping to their values
std::unordered_map<std::string, std::string>
parse_only_query(const std::string &what);
} // namespace query_string
} // namespace cobble
#endif