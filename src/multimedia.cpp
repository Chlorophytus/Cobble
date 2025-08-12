#include "../include/multimedia.hpp"
#include <boost/beast.hpp>
#include <exception>
#include <stdexcept>
using namespace cobble;

route::response_get
multimedia::thumbnail_get(const environment::configuration &config, U64 id) {
  route::response_get response{};

  response.mime_type = "image/webp";
  const auto path = (config.data_path / "thumbnails" / std::to_string(id))
                        .replace_extension("webp");
  response.body = boost::beast::http::file_body::value_type{};
  auto &&body =
      std::get<boost::beast::http::file_body::value_type>(response.body);

  boost::beast::error_code ec;
  body.open(path.c_str(), boost::beast::file_mode::scan, ec);

  if (ec) {
    throw std::runtime_error{ec.message()};
  }

  response.status = boost::beast::http::status::ok;

  return response;
}

route::response_head
multimedia::thumbnail_head(const environment::configuration &config, U64 id) {
  route::response_head response{};

  response.mime_type = "image/webp";
  const auto path = (config.data_path / "thumbnails" / std::to_string(id))
                        .replace_extension("webp");

  boost::beast::http::file_body::value_type body;

  boost::beast::error_code ec;
  body.open(path.c_str(), boost::beast::file_mode::scan, ec);

  if (ec) {
    throw std::runtime_error{ec.message()};
  }

  response.status = boost::beast::http::status::ok;
  response.size = body.size();

  return response;
};