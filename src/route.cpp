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
        {std::filesystem::path{"/.well-known/version"},
         [](std::unordered_map<std::string, std::string> &&query) {
           Json::Value root;

           root["ok"] = true;
           root["humanReadable"] = Cobble_VSTRING_FULL;
           root["machineReadable"]["major"] = Cobble_VMAJOR;
           root["machineReadable"]["minor"] = Cobble_VMINOR;
           root["machineReadable"]["patch"] = Cobble_VPATCH;

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