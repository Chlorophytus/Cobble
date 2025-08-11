#include "../include/route.hpp"
#include <functional>
using namespace cobble;

const static std::unordered_map<
    std::filesystem::path,
    std::function<route::response(
        std::unordered_map<std::string, std::string> &&)>>
    endpoints_get{
        {std::filesystem::path{"/"},
         [](std::unordered_map<std::string, std::string> &&query) {
           Json::Value root;

           root["ok"] = true;
           for (auto &&kv : query) {
             const auto [key, val] = kv;
             root["query"][key] = val;
           }

           return route::response{.status = boost::beast::http::status::ok,
                                  .body = root};
         }},
        {std::filesystem::path{"/page"},
         [](std::unordered_map<std::string, std::string> &&query) {
           Json::Value root;

           root["ok"] = true;
           root["version"]["readable"] = Cobble_VSTRING_FULL;
           root["version"]["major"] = Cobble_VMAJOR;
           root["version"]["minor"] = Cobble_VMINOR;
           root["version"]["patch"] = Cobble_VPATCH;
           root["videos"] = Json::arrayValue;

           return route::response{.status = boost::beast::http::status::ok,
                                  .body = root};
         }}};

route::response
route::api_get(const std::filesystem::path &path,
               std::unordered_map<std::string, std::string> &&query) {
  if (endpoints_get.contains(path)) {
    return endpoints_get.at(path)(std::move(query));
  } else {
    Json::Value root;

    root["ok"] = false;
    root["code"] = "NOT_FOUND";
    root["resource"] = path.string();

    return route::response{.status = boost::beast::http::status::not_found,
                           .body = root};
  }
}