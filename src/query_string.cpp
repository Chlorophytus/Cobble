#include "../include/query_string.hpp"
#include "../include/logger.hpp"
using namespace cobble;
#include <exception>
#include <forward_list>
#include <regex>
#include <stdexcept>

const static std::regex parse_path_query{R"(^([^&?]+)(?:\?([^?]*))?$)"};

std::unordered_map<std::string, std::string>
query_string::parse(const std::string &what, std::string &path) {
  std::unordered_map<std::string, std::string> parsed{};
  std::smatch path_matches;

  if (std::regex_search(what, path_matches, parse_path_query)) {
    // regex stuff, path and query splits here
    path = path_matches[1].str();
    std::string query = path_matches[2].str();

    // we don't need a vector
    std::forward_list<std::string> query_kvs{};

    auto where = query.find('&');

    // iterate through the query string
    do {
      query_kvs.emplace_front(query.substr(0, where));
      query = query.substr(where + 1);
      where = query.find('&');
    } while (where != std::string::npos);

    // place the last one in the front...
    query_kvs.emplace_front(query);

    // insert keys/values to the map
    for (const auto kv : query_kvs) {
      if (!kv.empty()) {
        const auto where = kv.find('=');
        const auto key = kv.substr(0, where);
        const auto val = kv.substr(where + 1);
        parsed.insert_or_assign(key, val);
      }
    }

    return parsed;
  } else {
    throw std::runtime_error{"An invalid path/query string was parsed"};
  }
}