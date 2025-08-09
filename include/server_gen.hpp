#if !defined(COBBLE_SERVER_GEN)
#define COBBLE_SERVER_GEN
#include "main.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
namespace cobble {
/// @brief Handles HTTP message generation
namespace server_gen {
/// @brief Generates a HTTP response
/// @tparam Body HTTP request body type
/// @tparam Allocator HTTP request allocator type
/// @param request the HTTP request
/// @return a message response
template <class Body, class Allocator>
boost::beast::http::message_generator
handle(boost::beast::http::request<
       Body, boost::beast::http::basic_fields<Allocator>> &&request) {
  // 400 bad request
  const auto bad_request = [&request](std::string_view reason) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::bad_request, request.version()};

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() = std::string(reason);

    response.prepare_payload();
    return response;
  };
  // 401 unauthorized
  const auto unauthorized = [&request](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::unauthorized, request.version()};

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() = "You are not authorized to access the resource '" +
                      std::string(where) + "'.";

    response.prepare_payload();
    return response;
  };
  // 403 forbidden
  const auto forbidden = [&request](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::forbidden, request.version()};

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() = "You are forbidden from accessing the resource '" +
                      std::string(where) + "'.";

    response.prepare_payload();
    return response;
  };
  // 404 not found
  const auto not_found = [&request](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::not_found, request.version()};

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() = "Could not find resource '" + std::string(where) + "'.";

    response.prepare_payload();
    return response;
  };
  // 500 internal server error
  const auto server_error = [&request](std::string_view what) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::internal_server_error, request.version()};

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.keep_alive(request.keep_alive());
    response.body() =
        "The server encountered an internal error: " + std::string(what) + ".";

    response.prepare_payload();
    return response;
  };

  // A message response body
  const std::string body{"Hello, C++\r\n"};
  const auto size = body.size();
  const auto method = request.method();

  switch (method) {
  case boost::beast::http::verb::head: {
    boost::beast::http::response<boost::beast::http::empty_body> response{
        boost::beast::http::status::ok, request.version()};
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.content_length(size);
    response.keep_alive(request.keep_alive());
    return response;
  }
  default: {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::ok, request.version()};
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "text/html");
    response.body() = body;
    response.content_length(size);
    response.keep_alive(request.keep_alive());
    return response;
  }
  }
}
} // namespace server_gen
} // namespace cobble
#endif