#include "../include/logger.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <sstream>
using namespace cobble;

static std::unique_ptr<logger::logger_list> ptr_loggers{nullptr};

void format_timestamp(std::stringstream &format) {
  constexpr auto BUFFER_SIZE = 128;
  format << "[";
  auto t = std::chrono::system_clock::now();
  auto t_time = std::chrono::system_clock::to_time_t(t);
  char t_buffer[BUFFER_SIZE]{0};
  std::strftime(t_buffer, BUFFER_SIZE - 1, "%T", std::localtime(&t_time));
  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                    t.time_since_epoch())
                    .count() %
                1000;
  format << t_buffer;
  format << ".";
  format << std::setw(3) << std::setfill('0') << millis;
  format << std::setw(0) << std::setfill(' ') << "] ";
}

logger::logger_list &logger::all_loggers() {
  if (!ptr_loggers) {
    ptr_loggers = std::make_unique<logger::logger_list>();
  }

  return *ptr_loggers;
}
// ============================================================================
void logger::stdout_listener::_prelude(std::string &s,
                                       const logger::severity severity) {
  std::stringstream format{};

  format_timestamp(format);
  switch (severity) {
  case logger::severity::emergency: {
    format << "\033[35m[ 0 | EMERGENCY ]\033[0m ";
    break;
  }
  case logger::severity::alert: {
    format << "\033[31m[ 1 | ALERT ]\033[0m ";
    break;
  }
  case logger::severity::critical: {
    format << "\033[31m[ 2 | CRITICAL ]\033[0m ";
    break;
  }
  case logger::severity::error: {
    format << "\033[33m[ 3 | ERROR ]\033[0m ";
    break;
  }
  case logger::severity::warning: {
    format << "\033[33m[ 4 | WARNING ]\033[0m ";
    break;
  }
  case logger::severity::notice: {
    format << "\033[32m[ 5 | NOTICE ]\033[0m ";
    break;
  }
  case logger::severity::informational: {
    format << "\033[0m[ 6 | INFORMATIONAL ] ";
    break;
  }
  case logger::severity::debug: {
    format << "\033[0m [ 7 | DEBUG ] ";
    break;
  }
  }

  s += format.str();
}

void logger::stdout_listener::_finalize(const std::string &s) {
  std::fprintf(stdout, "%s\033[0m\n", s.c_str());
}
// ============================================================================
logger::file_listener::file_listener(const std::filesystem::path &where) {
  _console_file = std::fopen(where.c_str(), "w");
}

void logger::file_listener::_prelude(std::string &s,
                                     const logger::severity severity) {
  std::stringstream format{};
  
  format_timestamp(format);
  switch (severity) {
  case logger::severity::emergency: {
    format << "[ 0 | EMERGENCY ] ";
    break;
  }
  case logger::severity::alert: {
    format << "[ 1 | ALERT ] ";
    break;
  }
  case logger::severity::critical: {
    format << "[ 2 | CRITICAL ] ";
    break;
  }
  case logger::severity::error: {
    format << "[ 3 | ERROR ] ";
    break;
  }
  case logger::severity::warning: {
    format << "[ 4 | WARNING ] ";
    break;
  }
  case logger::severity::notice: {
    format << "[ 5 | NOTICE ] ";
    break;
  }
  case logger::severity::informational: {
    format << "[ 6 | INFORMATIONAL ] ";
    break;
  }
  case logger::severity::debug: {
    format << "[ 7 | DEBUG ] ";
    break;
  }
  }

  s += format.str();
}

void logger::file_listener::_finalize(const std::string &s) {
  std::fprintf(_console_file, "%s\n", s.c_str());
}

logger::file_listener::~file_listener() { std::fclose(_console_file); }