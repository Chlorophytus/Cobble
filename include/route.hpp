#if !defined(COBBLE_ROUTE)
#define COBBLE_ROUTE
#include "environment.hpp"
#include "main.hpp"
#include <boost/beast.hpp>
#include <filesystem>
#include <json/json.h>
#include <optional>
#include <unordered_map>
#include <variant>
namespace cobble {
/// @brief Routes HTTP API paths to their appropriate handler
namespace route {
/// @brief A response for HEAD requests, with a HTTP status code
struct response_head {
  /// @brief HTTP status code
  boost::beast::http::status status;

  /// @brief The size of the file, or nothing
  std::optional<std::size_t> size = std::nullopt;

  /// @brief The MIME type of the file
  std::string mime_type;
};
/// @brief A JSON response for GET requests, with a HTTP status code
struct response_get {
  /// @brief HTTP status code
  boost::beast::http::status status;

  /// @brief The JSON or file response body
  std::variant<Json::Value, boost::beast::http::file_body::value_type> body;

  /// @brief The MIME type of this response
  std::string mime_type;
};
/// @brief A JSON response for POST requests, with a HTTP status code
struct response_post {
  /// @brief HTTP status code
  boost::beast::http::status status;

  /// @brief The JSON response body. Files don't get received by POSTing.
  Json::Value body;

  /// @brief The MIME type of this response
  std::string mime_type;
};
/// @brief Handle a HEAD request
/// @param config environment configuration
/// @param path the GET path
/// @param query the query string map
/// @return a response object
response_head api_head(const environment::configuration &config,
                       const std::filesystem::path &path,
                       std::unordered_map<std::string, std::string> &&query);

/// @brief Handle a GET request
/// @param config environment configuration
/// @param path the GET path
/// @param query the query string map
/// @return a response object
response_get api_get(const environment::configuration &config,
                     const std::filesystem::path &path,
                     std::unordered_map<std::string, std::string> &&query);

/// @brief Handle a POST request
/// @param config environment configuration
/// @param path the POST path
/// @param query the query string map
/// @param body the JSON body
/// @return a response object
response_post api_post(const environment::configuration &config,
                       const std::filesystem::path &path,
                       std::unordered_map<std::string, std::string> &&query,
                       Json::Value &&body);
} // namespace route
} // namespace cobble
#endif