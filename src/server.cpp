#include "../include/server.hpp"
#include "../include/logger.hpp"
#include "../include/server_gen.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <chrono>
#include <thread>
#include <vector>
using namespace cobble;

using tcp_stream = typename boost::beast::tcp_stream::rebind_executor<
    boost::asio::use_awaitable_t<>::executor_with_default<
        boost::asio::any_io_executor>>::other;

boost::asio::awaitable<void> do_session(tcp_stream stream) {
  const auto peer_ip = stream.socket().remote_endpoint().address().to_string();
  const auto peer_port = stream.socket().remote_endpoint().port();
  logger::log(logger::severity::debug, peer_ip, ":", peer_port, " connects");
  boost::beast::flat_buffer buffer;

  try {
    for (;;) {
      // set timeout
      stream.expires_after(std::chrono::seconds(3));

      // HTTP requests require a read of headers
      boost::beast::http::request<boost::beast::http::string_body> request;
      co_await boost::beast::http::async_read(stream, buffer, request);

      // handle request
      boost::beast::http::message_generator message =
          server_gen::handle(std::move(request));

      // determines if connection is done
      bool is_keepalive = message.keep_alive();

      // send response
      co_await boost::beast::async_write(stream, std::move(message),
                                         boost::asio::use_awaitable);

      if (!is_keepalive) {
        break;
      }
    }
  } catch (boost::system::system_error &e) {
    const auto code = e.code();

    if (code != boost::beast::http::error::end_of_stream) {
      if (code == boost::beast::error::timeout) {
        logger::log(logger::severity::warning, peer_ip, ":", peer_port,
                    " timed out on a request");
      } else {
        throw e;
      }
    }
  }

  logger::log(logger::severity::debug, peer_ip, ":", peer_port, " disconnects");
  boost::beast::error_code ec;
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}

boost::asio::awaitable<void>
do_listen(boost::asio::ip::tcp::endpoint endpoint) {
  auto acceptor =
      boost::asio::use_awaitable.as_default_on(boost::asio::ip::tcp::acceptor(
          co_await boost::asio::this_coro::executor));
  acceptor.open(endpoint.protocol());
  acceptor.set_option(boost::asio::socket_base::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen(boost::asio::socket_base::max_listen_connections);

  for (;;) {
    boost::asio::co_spawn(
        acceptor.get_executor(),
        do_session(tcp_stream{co_await acceptor.async_accept()}),
        [](std::exception_ptr e) {
          if (e) {
            std::rethrow_exception(e);
          }
        });
  }
}

void server::start(const environment::configuration &config) {
  boost::asio::io_context io_context{config.threads};

  boost::asio::co_spawn(io_context,
                        do_listen(boost::asio::ip::tcp::endpoint{
                            config.listen_address, config.listen_port}),
                        [](std::exception_ptr e) {
                          if (e) {
                            std::rethrow_exception(e);
                          }
                        });

  std::vector<std::thread> thread_pool{};
  thread_pool.reserve(config.threads - 1);

  for (auto thr = config.threads - 1; thr > 0; --thr) {
    thread_pool.emplace_back([&io_context] { io_context.run(); });
  }
  io_context.run();
}