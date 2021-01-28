///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   log_wrapper.hpp
/// @brief  日志模块的封装
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-18 21:53:30
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef INCLUDE_LOG_WRAPPER_HPP_
#define INCLUDE_LOG_WRAPPER_HPP_

#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "my_log/lazy_string.hpp"
#include "my_log/log.hpp"
#include "my_log/os.hpp"


namespace lee {
inline namespace log {
/// 默认文件刷新用的等级
constexpr lee::level_enum DEFAULT_FILE_LOG_LEVEL = lee::level_enum::debug;
constexpr lee::level_enum DEFAULT_COUT_LOG_LEVEL = lee::level_enum::debug;

class log_wrapper {
 public:
  static log_wrapper& get_instance() {
    static std::once_flag flag;
    static log_wrapper* instance;
    std::call_once(flag, [&]() { instance = new log_wrapper(); });
    return *instance;
  }

  /**
 * @name     write_log
 * @brief    主要进行C语言字符串整合为string型，
 *           传给日志打印函数，进行打印

 * @param    thread_id    [in]    线程ID
 * @param    func_name    [in]    调用该函数的函数名称
 * @param    line         [in]    在第几行调用该函数
 * @param    level        [in]    打印等级
 * @param    log          [in]    日志信息

 * @return   NONE
 * @author   Lijiancong, pipinstall@163.com
 * @date     2019-09-17 11:03:46
 * @warning  线程安全
 */
  void write_log(const std::thread::id thread_id, const std::string& file_name,
                 const std::string& func_name, const int line,
                 const lee::level_enum& level, const std::string& log) {
    auto formated_log =
        get_format_log(thread_id, file_name, func_name, line, level, log);
    base_log(level, formated_log);
  }

  void set_file_log_level(level_enum log_level) { logger.set_level(log_level); }

  void set_console_log_level(level_enum log_level) {
    cout_logger.set_level(log_level);
  }

  void set_flush_file_level(level_enum log_level) {
    file_flush_level_ = log_level;
  }

 private:
  log_wrapper() {
    logger.set_level(DEFAULT_FILE_LOG_LEVEL);
    cout_logger.set_level(DEFAULT_COUT_LOG_LEVEL);
  }
  ~log_wrapper() = default;
  log_wrapper(const log_wrapper&) = delete;
  log_wrapper operator=(const log_wrapper&) = delete;
  log_wrapper(log_wrapper&&) = delete;
  log_wrapper operator=(log_wrapper&&) = delete;
  void base_log(const lee::level_enum& level, const std::string& log) {
    if (cout_logger.should_log(level)) {
      cout_logger.log(log);
    }
    if (logger.should_log(level)) {
      logger.log(log);
    }
    if (file_flush_level_ <= level) {
      logger.flush();
    }
  }

  std::string get_format_log(const std::thread::id thread_id,
                             const std::string& file_name,
                             const std::string& func_name, const int line,
                             const lee::level_enum& level,
                             const std::string& log) {
    std::ostringstream oss;
    oss << thread_id;
    std::string stid = oss.str();
    std::string level_string = get_level_string(level);
#ifdef _WIN32
    char split = '\\';
#else
    char split = '/';
#endif

    std::string file = file_name;
    if (file_name.find_last_of(split) != std::string::npos) {
      file = file_name.substr(file_name.find_last_of(split) + 1);
    }

#ifdef USE_LAZY_STRING
    lee::lazy_string_concat_helper<> lazy_concat;
    std::string str_log =
        lazy_concat + lee::get_time_string() + " " + level_string + " " + log +
        " <In Function: " + func_name + "," + ", File: " + file +
        " Line: " + std::to_string(line) + ", PID: " + stid + ">\n";
#else
    std::string str_log =
        lee::get_time_string() + " " + level_string + " " + log +
        " <In Function: " + func_name + ", File: " + file +
        ", Line: " + std::to_string(line) + ", PID: " + stid + ">\n";
#endif
    return str_log;
  }

  std::string get_level_string(const lee::level_enum& level) {
    std::string str("[");
    if (level == lee::level_enum::trace) {
      str += "trace]";
    } else if (level == lee::level_enum::debug) {
      str += "debug]";
    } else if (level == lee::level_enum::info) {
      str += "info] ";
    } else if (level == lee::level_enum::warn) {
      str += "warn] ";
    } else if (level == lee::level_enum::error) {
      str += "error]";
    } else if (level == lee::level_enum::critical) {
      str += "critical]";
    } else {
      str += "unknow]";
    }
    return str;
  }

  lee::rotating_file_sink<std::mutex> logger;
  lee::stdout_sink<std::mutex> cout_logger;
  lee::level_enum file_flush_level_ = lee::level_enum::info;
};
}  // namespace log
template <typename T>
std::string to_log(const T& type) {
  std::ostringstream stream;
  stream << type;
  return stream.str();
}

inline std::string to_hex(const size_t dec) {
  std::ostringstream h;
  h << std::hex << (dec);
  std::string result("0x");
  result += h.str();
  return result;
}

template <typename T>
inline std::string pointer_to_hex(const T pointer) {
  static_assert(std::is_pointer<T>::value, "to_hex param is not a pointer!");
  return lee::to_hex(reinterpret_cast<size_t>(pointer));
}

inline std::string to_log(bool x) { return x ? "true" : "false"; }
template <typename T>
inline std::string to_log(T* x) {
  return lee::pointer_to_hex(x);
}

template <typename T>
inline std::string to_log(const std::vector<T>& vec) {
  std::string res("[");
  for (auto& it : vec) {
    res += lee::to_log(it);
    res += ",";
  }
  res.erase(std::prev(res.end()));
  res += "]";
  return res;
}

template <>
inline std::string to_log(const std::string& str) {
  return str;
}

template <>
inline std::string to_log(const char* const str) {
  std::string temp(str);
  return lee::to_log(temp);
}
}  // namespace lee

#define LOG_TRACE(x)                                              \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::trace, (_log_wrapper__ + (x)));        \
  } while (false)

#define LOG_DEBUG(x)                                              \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::debug, (_log_wrapper__ + (x)));        \
  } while (false)

#define LOG_INFO(x)                                               \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::info, (_log_wrapper__ + (x)));         \
  } while (false)

#define LOG_WARN(x)                                               \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::warn, (_log_wrapper__ + (x)));         \
  } while (false)

#define LOG_ERROR(x)                                              \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::error, (_log_wrapper__ + (x)));        \
  } while (false)

#define LOG_CRITICAL(x)                                           \
  do {                                                            \
    std::string _log_wrapper__;                                   \
    ::lee::log::log_wrapper::get_instance().write_log(            \
        std::this_thread::get_id(), __FILE__, __func__, __LINE__, \
        ::lee::level_enum::critical, (_log_wrapper__ + (x)));     \
  } while (false)

#endif
