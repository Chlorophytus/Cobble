#include "../include/server.hpp"
#include "../include/exception_handler.hpp"
#include "../include/logger.hpp"
#include "../include/server_gen.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <vector>
using namespace cobble;

using tcp_stream = typename boost::beast::tcp_stream::rebind_executor<
    boost::asio::use_awaitable_t<>::executor_with_default<
        boost::asio::any_io_executor>>::other;

boost::asio::awaitable<void>
do_session(tcp_stream stream, const environment::configuration &config) {

  const auto peer_ip = stream.socket().remote_endpoint().address().to_string();
  const auto peer_port = stream.socket().remote_endpoint().port();
  logger::log(logger::severity::debug, peer_ip, ":", peer_port, " connects");
  boost::beast::flat_buffer buffer;

  try {
    for (;;) {
      // set timeout
      stream.expires_after(std::chrono::seconds(1));

      // HTTP requests require a read of headers
      boost::beast::http::request<boost::beast::http::string_body> request;
      co_await boost::beast::http::async_read(stream, buffer, request);

      // handle request
      boost::beast::http::message_generator message =
          server_gen::handle(std::move(request), config, peer_ip, peer_port);

      // determines if connection is done
      bool is_keepalive = message.keep_alive();

      // send response
      co_await boost::beast::async_write(stream, std::move(message),
                                         boost::asio::use_awaitable);

      if (!is_keepalive) {
        logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                    " done with sending");
        break;
      }
    }
  } catch (boost::system::system_error &e) {
    const auto code = e.code();

    if (code == boost::asio::error::operation_aborted) {
      // https://github.com/boostorg/beast/issues/2145#issuecomment-755445748
      logger::log(logger::severity::debug, peer_ip, ":", peer_port,
                  " probably did not close their connection on time, "
                  "disconnecting anyway");
      co_return;
    }

    if (code != boost::beast::http::error::end_of_stream) {
      throw e;
    }
  }

  logger::log(logger::severity::debug, peer_ip, ":", peer_port, " disconnects");
  stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);
}

boost::asio::awaitable<void>
do_listen(boost::asio::ip::tcp::endpoint endpoint,
          const environment::configuration &config) {
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
        do_session(tcp_stream{co_await acceptor.async_accept()}, config),
        [](std::exception_ptr e) {
          if (e) {
            std::rethrow_exception(e);
          }
        });
  }
}

void server::start(const environment::configuration &config,
                   std::atomic<bool> &run) {
  logger::log(logger::severity::notice, "Spinning up server with ",
              config.threads, " threads...");
  boost::asio::io_context io_context{config.threads};

  boost::asio::co_spawn(
      io_context,
      do_listen(boost::asio::ip::tcp::endpoint{config.listen_address,
                                               config.listen_port},
                config),
      [&io_context](std::exception_ptr e) {
        if (e) {
          std::rethrow_exception(e);
        }
      });

  std::vector<std::thread> thread_pool{};
  thread_pool.reserve(config.threads - 1);

  for (auto thr = config.threads - 1; thr > 0; --thr) {
    thread_pool.emplace_back([&io_context] { io_context.run(); });
  }

  // Stops all server threads
  auto try_halt = [&io_context, &thread_pool]() {
    io_context.stop();
    logger::log(logger::severity::informational,
                "Spinning down server...");
    for (auto &&thr : thread_pool) {
      thr.join();
    }
  };

  try {
    while (run) {
      io_context.poll();
    }
  } catch (const std::exception &e) {
    logger::log(logger::severity::error, "Error from a thread was caught, spinning down server");
    try_halt();
    std::throw_with_nested(std::runtime_error{"Server spun down ungracefully"});
  }

  try_halt();
}