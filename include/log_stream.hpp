///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
///
/// @file   log_stream.hpp
/// @brief  日志文件
///
/// @author lijiancong, pipinstall@163.com
/// @date   2021-01-08 13:13:01
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef MY_LOG_INCLUDE_LOG_STREAM_H_
#define MY_LOG_INCLUDE_LOG_STREAM_H_

#include <sstream>
#include <string>
#include <thread>

#include "log_wrapper.hpp"

#undef TRACE
#undef DEBUG
#undef INFO
#undef WARN
#undef ERROR
#undef CRITICAL
#undef OFF

enum log_level_enum {
  TRACE = static_cast<unsigned>(lee::level_enum::trace),
  DEBUG = static_cast<unsigned>(lee::level_enum::debug),
  INFO = static_cast<unsigned>(lee::level_enum::info),
  WARN = static_cast<unsigned>(lee::level_enum::warn),
  ERROR = static_cast<unsigned>(lee::level_enum::error),
  CRITICAL = static_cast<unsigned>(lee::level_enum::critical),
  OFF = static_cast<unsigned>(lee::level_enum::trace),
};

namespace lee {
inline namespace log {
class log_stream {
 public:
  log_stream(const log_level_enum& level, const std::thread::id thread_id,
             const std::string& file_name, const std::string& func_name,
             const int line)
      : level_(level),
        thread_id_(thread_id),
        file_name_(file_name),
        func_name_(func_name),
        line_(line) {}

  ~log_stream() {
    if (log_.empty()) {
      return;
    }

    if (TRACE == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_, ::lee::level_enum::trace,
          log_);
    } else if (DEBUG == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_, ::lee::level_enum::debug,
          log_);
    } else if (INFO == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_, ::lee::level_enum::info,
          log_);
    } else if (WARN == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_, ::lee::level_enum::warn,
          log_);
    } else if (ERROR == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_, ::lee::level_enum::error,
          log_);
    } else if (CRITICAL == level_) {
      ::lee::log::log_wrapper::get_instance().write_log(
          thread_id_, file_name_, func_name_, line_,
          ::lee::level_enum::critical, log_);
    } else if (OFF == level_) {
      /// DONOTHING
    } else {
      /// DONOTHING
    }
  }

  log_stream(const log_stream& other)
      : log_(other.log_),
        level_(other.level_),
        thread_id_(other.thread_id_),
        file_name_(other.file_name_),
        func_name_(other.func_name_),
        line_(other.line_) {}

  log_stream& operator=(const log_stream& other) = delete;

  template <typename T>
  log_stream& operator<<(const T& data) {
    std::stringstream stream;
    stream << data;
    log_ += stream.str();
    return *this;
  }

  log_stream& operator<<(const bool& data) {
    log_ += data ? "true" : "false";
    return *this;
  }

 private:
  std::string log_;
  const log_level_enum level_;
  const std::thread::id thread_id_;
  const std::string file_name_;
  const std::string func_name_;
  const int line_;
};

inline ::lee::log::log_stream log_stream_helper(const log_level_enum& level,
                                                const std::thread::id thread_id,
                                                const std::string& file_name,
                                                const std::string& func_name,
                                                const int line) {
  return ::lee::log::log_stream(level, thread_id, file_name, func_name, line);
}

}  // namespace log
}  // namespace lee

#define LOG(X)                                                           \
  ::lee::log::log_stream_helper(X, std::this_thread::get_id(), __FILE__, \
                                __func__, __LINE__)

#endif  // end of MY_LOG_INCLUDE_LOG_STREAM_H_
