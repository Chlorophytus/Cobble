#if !defined(COBBLE_SERVER_GEN)
#define COBBLE_SERVER_GEN
#include "environment.hpp"
#include "main.hpp"
#include "query_string.hpp"
#include "route.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <chrono>
#include <forward_list>
#include <json/json.h>
#include <variant>
namespace cobble {
/// @brief Handles HTTP message generation
namespace server_gen {
/// @brief Generates a HTTP response
/// @tparam Body HTTP request body type
/// @tparam Allocator HTTP request allocator type
/// @param request The HTTP request
/// @param config A listener configuration
/// @param peer_ip The peer IP address
/// @param peer_port The peer port
/// @return a message response
template <class Body, class Allocator>
boost::beast::http::message_generator
handle(boost::beast::http::request<
           Body, boost::beast::http::basic_fields<Allocator>> &&request,
       const environment::configuration &config, const std::string &peer_ip,
       const U16 peer_port) {
  // initial handle time
  std::chrono::high_resolution_clock::time_point t0 =
      std::chrono::high_resolution_clock::now();

  // 500 internal server error
  const auto server_error = [&request, &peer_ip, &peer_port, &t0] {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::internal_server_error, request.version()};
    logger::log(logger::severity::warning, peer_ip, ":", peer_port,
                " returns HTTP 500");
    response.set(boost::beast::http::field::access_control_allow_origin,
                 request["origin"]);
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "SERVER_ERROR";
    root["maintenanceMessage"] = "Please try again later";
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
    root["responseTime"] =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };
  // 400 bad request
  const auto bad_request = [&request, &peer_ip, &peer_port, &t0] {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::bad_request, request.version()};

    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 400");
    response.set(boost::beast::http::field::access_control_allow_origin,
                 request["origin"]);
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "BAD_REQUEST";
    root["maintenanceMessage"] = "Please try again later";
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
    root["responseTime"] =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };

  // 401 unauthorized
  const auto unauthorized = [&request, &peer_ip, &peer_port, &t0] {
    boost::beast::http::response<boost::beast::http::string_body> response{
        boost::beast::http::status::unauthorized, request.version()};

    logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                " returns HTTP 401");
    response.set(boost::beast::http::field::access_control_allow_origin,
                 request["origin"]);
    response.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
    response.set(boost::beast::http::field::content_type, "application/json");
    response.keep_alive(request.keep_alive());

    Json::Value root;
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "";

    root["ok"] = false;
    root["code"] = "UNAUTHORIZED";
    root["maintenanceMessage"] = "Can't access the API";
    std::chrono::high_resolution_clock::time_point t1 =
        std::chrono::high_resolution_clock::now();
    root["responseTime"] =
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    response.body() = Json::writeString(builder, root);

    response.prepare_payload();
    return response;
  };

  try {
    // Ensure CORS is not blocked here
    bool in_network = false;

    if (std::holds_alternative<std::forward_list<std::string>>(
            config.cors_entries)) {
      // holding DNS entries
      const auto &cors =
          std::get<std::forward_list<std::string>>(config.cors_entries);
      for (const auto &origin : cors) {
        if (request["origin"] == origin || origin == "*") {
          in_network = true;
          break;
        }
      }
    } else {
      // holding IP address ranges
      const auto ip_converted = boost::asio::ip::make_address(peer_ip);
      const auto &cors =
          std::get<environment::cidr_network_list>(config.cors_entries);

      if (ip_converted.is_v4()) {
        // peer is IPv4 so we need to use IPv4 ranges
        for (const auto &cidr : cors.v4) {
          auto hosts = cidr.hosts();
          if (hosts.find(ip_converted.to_v4()) != hosts.end()) {
            in_network = true;
            break;
          }
        }
      } else {
        // peer is IPv6 so we need to use IPv6 ranges
        for (const auto &cidr : cors.v6) {
          auto hosts = cidr.hosts();
          if (hosts.find(ip_converted.to_v6()) != hosts.end()) {
            in_network = true;
            break;
          }
        }
      }
    }

    if (!in_network) {
      return unauthorized();
    }

    const auto target = request.target();
    std::filesystem::path target_path{};
    auto parsed = query_string::parse(target, target_path);

    logger::log(logger::severity::debug, peer_ip, ":", peer_port, " reads '",
                target, "' ", request.method_string());

    const auto method = request.method();

    switch (method) {
    case boost::beast::http::verb::head: {
      auto routed = route::api_head(config, target_path, std::move(parsed));

      boost::beast::http::response<boost::beast::http::empty_body> response{
          boost::beast::http::status::ok, request.version()};
      response.set(boost::beast::http::field::access_control_allow_origin,
                   request["origin"]);
      response.set(boost::beast::http::field::server,
                   BOOST_BEAST_VERSION_STRING);
      response.set(boost::beast::http::field::content_type, routed.mime_type);
      response.content_length(routed.size.value_or(0));
      response.keep_alive(request.keep_alive());
      std::chrono::high_resolution_clock::time_point t1 =
          std::chrono::high_resolution_clock::now();
      response.set(
          "X-Response-Time",
          std::to_string(
              std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0)
                  .count()));

      return response;
    }
    case boost::beast::http::verb::get: {
      auto routed = route::api_get(config, target_path, std::move(parsed));

      if (std::holds_alternative<Json::Value>(routed.body)) {
        auto &&body_json = std::get<Json::Value>(routed.body);
        boost::beast::http::response<boost::beast::http::string_body> response{
            routed.status, request.version()};
        response.set(boost::beast::http::field::access_control_allow_origin,
                     request["origin"]);
        response.set(boost::beast::http::field::server,
                     BOOST_BEAST_VERSION_STRING);
        response.set(boost::beast::http::field::content_type, routed.mime_type);
        response.keep_alive(request.keep_alive());

        Json::StreamWriterBuilder builder;
        builder.settings_["indentation"] = "";
        std::chrono::high_resolution_clock::time_point t1 =
            std::chrono::high_resolution_clock::now();
        body_json["responseTime"] =
            std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0)
                .count();
        response.body() = Json::writeString(builder, body_json);
        response.prepare_payload();
        return response;
      } else {
        auto &&body_file =
            std::get<boost::beast::http::file_body::value_type>(routed.body);
        boost::beast::http::response<boost::beast::http::file_body> response{
            routed.status, request.version()};
        response.set(boost::beast::http::field::access_control_allow_origin,
                     request["origin"]);
        response.set(boost::beast::http::field::server,
                     BOOST_BEAST_VERSION_STRING);
        response.set(boost::beast::http::field::content_type, routed.mime_type);
        response.keep_alive(request.keep_alive());
        response.body() = std::move(body_file);
        std::chrono::high_resolution_clock::time_point t1 =
            std::chrono::high_resolution_clock::now();
        response.set(
            "X-Response-Time",
            std::to_string(
                std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0)
                    .count()));
        response.prepare_payload();
        return response;
      }
    }
    default: {
      return bad_request();
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