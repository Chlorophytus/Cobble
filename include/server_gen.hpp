#if !defined(COBBLE_SERVER_GEN)
#define COBBLE_SERVER_GEN
#include "main.hpp"
#include "query_string.hpp"
#include "route.hpp"
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
       const std::string &cors, const std::string &peer_ip,
       const U16 peer_port) {
  // 500 internal server error
  const auto server_error = [&request, &peer_ip, &peer_port, &cors]() {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::internal_server_error, request.version()};
    logger::log(logger::severity::warning, peer_ip, ":", peer_port,
                " returns HTTP 500");
    response.set(boost::beast::http::field::access_control_allow_origin, cors);
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "SERVER_ERROR";
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 400 bad request
  const auto bad_request = [&request, &peer_ip,
                            &peer_port, &cors](std::string_view reason) {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::bad_request, request.version()};

    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 400");
    response.set(boost::beast::http::field::access_control_allow_origin, cors);
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

  try {
    const auto target = request.target();
    std::filesystem::path target_path{};
    auto parsed = query_string::parse(target, target_path);

    logger::log(logger::severity::debug, peer_ip, ":", peer_port, " reads '",
                target, "' ", request.method_string());

    const auto method = request.method();

    switch (method) {
    case boost::beast::http::verb::head: {
      boost::beast::http::response<boost::beast::http::empty_body> response{
          boost::beast::http::status::ok, request.version()};
      response.set(boost::beast::http::field::access_control_allow_origin,
                   cors);
      response.set(boost::beast::http::field::server,
                   BOOST_BEAST_VERSION_STRING);
      response.set(boost::beast::http::field::content_type, "application/json");
      response.keep_alive(request.keep_alive());

      response.prepare_payload();
      return response;
    }
    case boost::beast::http::verb::get: {
      const auto routed = route::api_get(target_path, std::move(parsed));

      boost::beast::http::response<boost::beast::http::string_body> response{
          routed.status, request.version()};
      response.set(boost::beast::http::field::access_control_allow_origin,
                   cors);
      response.set(boost::beast::http::field::server,
                   BOOST_BEAST_VERSION_STRING);
      response.set(boost::beast::http::field::content_type, "application/json");
      response.keep_alive(request.keep_alive());

      Json::StreamWriterBuilder builder;
      builder.settings_["indentation"] = "";
      response.body() = Json::writeString(builder, routed.body);

      response.prepare_payload();
      return response;
    }
    default: {
      return bad_request("Bad HTTP request type");
    }
    }
  } catch (const std::exception &e) {
    logger::log(logger::severity::error,
                "HTTP generator errored, printing stacktrace");
    exception_handler::print_nested(e);
    return server_error();
  }
}
} // namespace server_gen
} // namespace cobble
#endif