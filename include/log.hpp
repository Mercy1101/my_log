///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   log.hpp
/// @brief  日志文件
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-18 11:39:27
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef MY_LOG_INCLUDE_LOG_H_
#define MY_LOG_INCLUDE_LOG_H_

#include "file_helper.hpp"
#include <atomic>
#include <mutex>
#include <string>

namespace lee {
inline namespace log {

enum class level_enum {
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  error = 4,
  critical = 5,
  off = 6,
  n_levels
};

class sink {
 public:
  virtual ~sink() = default;
  virtual void log(const std::string &msg) = 0;
  virtual void flush() = 0;

  inline bool should_log(level_enum msg_level) const {
    /// return msg_level >= level_.load(std::memory_order_relaxed);
    return static_cast<int>(msg_level) >= static_cast<int>(level_);
  }

  inline void set_level(level_enum log_level) {
    /// level_.store(log_level, std::memory_order_relaxed);
    level_ = log_level;
  }

  inline level_enum level() const {
    /// return static_cast<level_enum>(level_.load(std::memory_order_relaxed));
    return level_;
  }

 protected:
  // sink log level - default is all
  level_enum level_{level_enum::trace};
};

template <typename Mutex>
class base_sink : public sink {
 public:
  base_sink()   {}
  ~base_sink() override = default;

  base_sink(const base_sink &) = delete;
  base_sink(base_sink &&) = delete;

  base_sink &operator=(const base_sink &) = delete;
  base_sink &operator=(base_sink &&) = delete;

  void log(const std::string & msg) final {
    std::lock_guard<Mutex> lock(mutex_);
    sink_it_(msg);
  }
  void flush() final {
    std::lock_guard<Mutex> lock(mutex_);
    flush_();
  }

 protected:
  Mutex mutex_;

  virtual void sink_it_(const std::string &msg) = 0;
  virtual void flush_() = 0;
};

template <typename Mutex>
class rotating_file_sink final : public base_sink<Mutex> {
 public:
  rotating_file_sink(std::string base_filename = std::string("log/detail/detail_log.log"), std::size_t max_size = 1048576*50,
                     std::size_t max_files = 10, bool rotate_on_open = false)
      : base_filename_(std::move(base_filename)),
        max_size_(max_size),
        max_files_(max_files) {
    file_helper_.open(calc_filename(base_filename_, 0));
    current_size_ = file_helper_.size();  // expensive. called only once
    if (rotate_on_open && current_size_ > 0) {
      rotate_();
    }
  }

  static inline std::string calc_filename(const std::string &filename,
                                          std::size_t index) {
    if (index == 0u) {
      return filename;
    }

    std::string basename, ext;
    std::tie(basename, ext) = file_helper::split_by_extension(filename);
    return basename + std::to_string(index) + "." + ext;
  }
  template <typename Mutex>
  inline std::string filename() {
    std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
    return file_helper_.filename();
  }

 protected:
  void sink_it_(const std::string &msg) override {
    /// std::string formatted;
    /// base_sink<Mutex>::formatter_->format(msg, formatted);
    current_size_ += msg.size();
    if (current_size_ > max_size_) {
      rotate_();
      current_size_ = msg.size();
    }
    file_helper_.write(msg);
  }
  void flush_() override { file_helper_.flush(); }

 private:
  // Rotate files:
  // log.txt -> log.1.txt
  // log.1.txt -> log.2.txt
  // log.2.txt -> log.3.txt
  // log.3.txt -> delete
  inline void rotate_() {
    /// using std::stringo_str;
    /// using path_exists;
    file_helper_.close();
    for (auto i = max_files_; i > 0; --i) {
      std::string src = calc_filename(base_filename_, i - 1);
      if (!path_exists(src)) {
        continue;
      }
      std::string target = calc_filename(base_filename_, i);

      if (!rename_file_(src, target)) {
        // if failed try again after a small delay.
        // this is a workaround to a windows issue, where very high rotation
        // rates can cause the rename to fail with permission denied (because of
        // antivirus?).
        sleep_for_millis(100);
        if (!rename_file_(src, target)) {
          file_helper_.reopen(true);  // truncate the log file anyway to prevent
                                      // it to grow beyond its limit!
          current_size_ = 0;
          throw("rotating_file_sink: failed renaming " + (src) +
                    " to " + (target));
        }
      }
    }
    file_helper_.reopen(true);
  }

  // delete the target if exists, and rename the src file  to target
  // return true on success, false otherwise.
  inline bool rename_file_(const std::string &src_filename,
                           const std::string &target_filename) {
    // try to delete the target file in case it already exists.
    (void)lee::os::remove(target_filename);
    return lee::os::rename(src_filename, target_filename) == 0;
  }

  std::string base_filename_;
  std::size_t max_size_;
  std::size_t max_files_;
  std::size_t current_size_;
  file_helper file_helper_;  /// 用于打开、写文件的对象
};

}  // namespace log
}  // namespace lee

#endif  // end of MY_LOG_INCLUDE_LOG_H_
