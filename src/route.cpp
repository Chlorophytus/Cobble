#include "../include/route.hpp"
#include "../include/multimedia.hpp"
#include <functional>
using namespace cobble;

const static std::unordered_map<
    std::filesystem::path,
    std::function<route::response_get(
        const environment::configuration &,
        std::unordered_map<std::string, std::string> &&)>>
    endpoints_get{
        {std::filesystem::path{"/page"},
         [](const environment::configuration &config,
            std::unordered_map<std::string, std::string> &&query) {
           Json::Value root;

           root["ok"] = true;
           root["version"]["readable"] = Cobble_VSTRING_FULL;
           root["version"]["major"] = Cobble_VMAJOR;
           root["version"]["minor"] = Cobble_VMINOR;
           root["version"]["patch"] = Cobble_VPATCH;
           root["videos"] = Json::arrayValue;

           return route::response_get{.status = boost::beast::http::status::ok,
                                      .body = root,
                                      .mime_type = "application/json"};
         }},
        {std::filesystem::path{"/thumb"},
         [](const environment::configuration &config,
            std::unordered_map<std::string, std::string> &&query) {
           if (query.contains("idx")) {
             return multimedia::thumbnail_get(config,
                                              std::stoull(query.at("idx")));
           } else {
             Json::Value root;

             root["ok"] = false;
             root["code"] = "BAD_THUMBNAIL";

             return route::response_get{
                 .status = boost::beast::http::status::bad_request,
                 .body = root,
                 .mime_type = "application/json"};
           }
         }}};

const static std::unordered_map<
    std::filesystem::path,
    std::function<route::response_head(
        const environment::configuration &,
        std::unordered_map<std::string, std::string> &&)>>
    endpoints_head{
        {std::filesystem::path{"/page"},
         [](const environment::configuration &config,
            std::unordered_map<std::string, std::string> &&query) {
           return route::response_head{.status = boost::beast::http::status::ok,
                                       .mime_type = "application/json"};
         }},
        {std::filesystem::path{"/thumb"},
         [](const environment::configuration &config,
            std::unordered_map<std::string, std::string> &&query) {
           if (query.contains("idx")) {
             return multimedia::thumbnail_head(config,
                                               std::stoull(query.at("idx")));
           } else {
             return route::response_head{
                 .status = boost::beast::http::status::bad_request,
                 .mime_type = "application/json"};
           }
         }}
    };

route::response_get
route::api_get(const environment::configuration &config,
               const std::filesystem::path &path,
               std::unordered_map<std::string, std::string> &&query) {
  if (endpoints_get.contains(path)) {
    return endpoints_get.at(path)(config, std::move(query));
  } else {
    Json::Value root;

    root["ok"] = false;
    root["code"] = "NOT_FOUND";
    root["maintenanceMessage"] = "Please try again later";
    root["resource"] = path.string();

    return route::response_get{.status = boost::beast::http::status::not_found,
                               .body = root,
                               .mime_type = "application/json"};
  }
}
route::response_head
route::api_head(const environment::configuration &config,
                const std::filesystem::path &path,
                std::unordered_map<std::string, std::string> &&query) {
  if (endpoints_head.contains(path)) {
    return endpoints_head.at(path)(config, std::move(query));
  } else {

    return route::response_head{.status = boost::beast::http::status::not_found,
                                .mime_type = "application/json"};
  }
}