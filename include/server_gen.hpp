#if !defined(COBBLE_SERVER_GEN)
#define COBBLE_SERVER_GEN
#include "main.hpp"
#include "query_string.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <json/json.h>
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
           Body, boost::beast::http::basic_fields<Allocator>> &&request,
       const std::string &peer_ip, const U16 peer_port) {
  // 400 bad request
  const auto bad_request = [&request, &peer_ip,
                            &peer_port](std::string_view reason) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::bad_request, request.version()};

    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 400");
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "BAD_REQUEST";
    root["reason"] = std::string(reason);
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 401 unauthorized
  const auto unauthorized = [&request, &peer_ip,
                             &peer_port](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::unauthorized, request.version()};
    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 401");

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "UNAUTHORIZED";
    root["resource"] = std::string(where);
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 403 forbidden
  const auto forbidden = [&request, &peer_ip,
                          &peer_port](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::forbidden, request.version()};
    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 403");

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "FORBIDDEN";
    root["resource"] = std::string(where);
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 404 not found
  const auto not_found = [&request, &peer_ip,
                          &peer_port](std::string_view where) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::not_found, request.version()};
    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 404");

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "NOT_FOUND";
    root["resource"] = std::string(where);
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 500 internal server error
  const auto server_error = [&request, &peer_ip,
                             &peer_port](std::string_view what) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::internal_server_error, request.version()};
    logger::log(logger::severity::warning, peer_ip, ":", peer_port,
                " returns HTTP 500");

    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "SERVER_ERROR";
    root["reason"] = std::string(what);
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };

  const auto target = request.target();
    logger::log(logger::severity::debug, peer_ip, ":", peer_port, " reads '", target, "' ", request.method_string());

  std::string target_path{};
  const auto parsed = query_string::parse(target, target_path);
  if (target_path == "/") {
    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = true;
    root["query"] = Json::Value{};
    for(auto &&kv : parsed) {
      const auto [key, val] = kv;
      root["query"][key] = val;
    }

    // A message response body
    const auto method = request.method();
    switch (method) {
    case boost::beast::http::verb::head: {
      boost::beast::http::response<boost::beast::http::empty_body> response{
          boost::beast::http::status::ok, request.version()};
      response.set(boost::beast::http::field::server,
                   BOOST_BEAST_VERSION_STRING);
      response.set(boost::beast::http::field::content_type, "application/json");
      response.keep_alive(request.keep_alive());

      response.prepare_payload();
      return response;
    }
    case boost::beast::http::verb::get: {
      boost::beast::http::response<boost::beast::http::string_body> response{
          boost::beast::http::status::ok, request.version()};
      response.set(boost::beast::http::field::server,
                   BOOST_BEAST_VERSION_STRING);
      response.set(boost::beast::http::field::content_type, "application/json");
      response.body() = Json::writeString(builder, root);
      response.keep_alive(request.keep_alive());

      response.prepare_payload();
      return response;
    }
    default: {
      return bad_request("Bad HTTP request type");
    }
    }
  } else {
    return not_found(target_path);
  }
}
} // namespace server_gen
} // namespace cobble
#endif