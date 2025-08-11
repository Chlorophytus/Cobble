#if !defined(COBBLE_ROUTE)
#define COBBLE_ROUTE
#include "main.hpp"
#include <boost/beast.hpp>
#include <filesystem>
#include <json/json.h>
#include <unordered_map>
namespace cobble {
/// @brief Routes HTTP API paths to their appropriate handler
namespace route {
/// @brief A JSON response with a HTTP status code
struct response {
  /// @brief HTTP status code
  boost::beast::http::status status;

  /// @brief The JSON response body
  Json::Value body;
};
/// @brief Handle a GET request
/// @param path the GET path
/// @param query the query string map
/// @return a response object
response api_get(const std::filesystem::path &path,
                 std::unordered_map<std::string, std::string> &&query);

/// @brief Handle a POST request
/// @param path the POST path
/// @param query the query string map
/// @param body the JSON body
/// @return a response object
response api_post(const std::filesystem::path &path,
                  std::unordered_map<std::string, std::string> &&query,
                  Json::Value &&body);
} // namespace route
} // namespace cobble
#endif