#if !defined(COBBLE_LOGGER)
#define COBBLE_LOGGER
#include "main.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
namespace cobble {
/// @brief Handles logging to I/O devices
namespace logger {
/// @brief The severity of an event, in RFC5424 context
enum class severity : U8 {
  /// @brief System is unusable
  emergency = 0,

  /// @brief Action must be taken immediately
  alert = 1,

  /// @brief Critical conditions
  critical = 2,

  /// @brief Error conditions
  error = 3,

  /// @brief Warning conditions
  warning = 4,

  /// @brief Normal but significant condition
  notice = 5,

  /// @brief Informational messages
  informational = 6,

  /// @brief Debug-evel messages
  debug = 7
};

/// @brief An Abstract Base Class for a listener logger
class base_listener {
  std::string _make_string(void *head) {
    std::stringstream hex;
    hex << std::hex << "0x" << head;
    return hex.str();
  }
  std::string _make_string(std::filesystem::path head) { return head.string(); }
  std::string _make_string(std::string head) { return head; }
  std::string _make_string(const char *head) { return head; }
  std::string _make_string(bool head) { return head ? "true" : "false"; }
  std::string _make_string(U8 head) { return std::to_string(head); }
  std::string _make_string(U16 head) { return std::to_string(head); }
  std::string _make_string(U32 head) { return std::to_string(head); }
  std::string _make_string(U64 head) { return std::to_string(head); }
  std::string _make_string(S8 head) { return std::to_string(head); }
  std::string _make_string(S16 head) { return std::to_string(head); }
  std::string _make_string(S32 head) { return std::to_string(head); }
  std::string _make_string(S64 head) { return std::to_string(head); }
  std::string _make_string(F32 head) { return std::to_string(head); }
  std::string _make_string(F64 head) { return std::to_string(head); }

  virtual void _prelude(std::string &, const severity) = 0;
  virtual void _finalize(const std::string &) = 0;

public:
  /// @brief Log to this individual logger
  /// @tparam ...Ts Template parameter pack for `params`
  /// @param severity Log priority/severity
  /// @param ...params Parameters to log
  template <class... Ts> void log(const severity severity, Ts... params) {
    std::string s{};
    _prelude(s, severity);
    s += (... + _make_string(params));
    _finalize(s);
  }

  /// @brief Cleans up the listener
  virtual ~base_listener() {};
};

/// @brief Standard output I/O listener
class stdout_listener : public base_listener {
  void _prelude(std::string &, const severity) override;
  void _finalize(const std::string &) override;
};

/// @brief Log file I/O listener
class file_listener : public base_listener {
  FILE *_console_file;
  void _prelude(std::string &, const severity) override;
  void _finalize(const std::string &) override;

public:
  /// @brief Opens the file for logging
  /// @param where The file that we log to
  file_listener(const std::filesystem::path &where);

  /// @brief Closes the log file
  ~file_listener() override;
};

/// @brief A list of log listener pointers
using logger_list = std::vector<std::unique_ptr<base_listener>>;

/// @brief Get a reference to the global log listeners container
/// @return The reference to the log listeners container
logger_list &all_loggers();

/// @brief Logs a message to all available loggers
/// @tparam ...Ts Template parameter pack for `params`
/// @param severity Log priority/severity
/// @param ...params Parameters to log
template <class... Ts> void log(const severity severity, Ts... params) {
  logger_list &loggers = all_loggers();
  for (auto &logger : loggers) {
    logger->log(severity, params...);
  }
}
} // namespace logger
} // namespace cobble
#endif