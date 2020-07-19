///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   file_helper.hpp
/// @brief  文件操作相关的函数
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-18 15:35:21
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef MY_LOG_INCLUDE_FILE_HELPER_H_
#define MY_LOG_INCLUDE_FILE_HELPER_H_

#include <string>

#include "os.hpp"

namespace lee {
inline namespace my_log {

/// @name     file_helper
/// @brief    用于实现循环写文件的操作
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:37:55
/// @warning  线程不安全
class file_helper {
 public:
  explicit file_helper() = default;
  file_helper(const file_helper &) = delete;
  file_helper &operator=(const file_helper &) = delete;
  inline ~file_helper() { close(); }

  /// @name     open
  /// @brief    打开一个文件
  ///
  /// @param    fname     [in]  文件名称
  /// @param    truncate  [in]  文件是否覆写
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:01:09
  /// @warning  线程不安全
  inline void open(const std::string &fname, bool truncate = false) {
    close();
    filename_ = fname;
    auto *mode = truncate ? ("wb") : ("ab");

    for (int tries = 0; tries < open_tries_; ++tries) {
      // create containing folder if not exists already.
      lee::create_dir(dir_name(fname));
      if (!lee::os::fopen_s(&fd_, fname.c_str(), mode)) {
        return;
      }

      lee::sleep_for_millis(open_interval_);
    }

    throw("Failed opening file " + filename_ + " for writing");
  }

  /// @name     reopen
  /// @brief    重新打开一个文件
  ///
  /// @param    truncate  [in]  是否覆写
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:02:45
  /// @warning  线程不安全
  inline void reopen(bool truncate) {
    if (filename_.empty()) {
      throw("Failed re opening file - was not opened before");
    }
    this->open(filename_, truncate);
  }

  /// @name     flush
  /// @brief    刷新文件流
  ///
  /// @param    NONE
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:04:12
  /// @warning  线程不安全
  inline void flush() { std::fflush(fd_); }

  /// @name     close
  /// @brief    关闭一个文件
  ///
  /// @param    NONE
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:04:49
  /// @warning  线程不安全
  inline void close() {
    if (fd_ != nullptr) {
      std::fclose(fd_);
      fd_ = nullptr;
    }
  }

  /// @name     write
  /// @brief    向文件中写入
  ///
  /// @param    buf [in]  要写入的string
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:05:29
  /// @warning  线程不安全
  inline void write(const std::string &buf) {
    size_t msg_size = buf.size();
    auto data = buf.data();
    if (std::fwrite(data, 1, msg_size, fd_) != msg_size) {
      throw("Failed writing to file " + (filename_));
    }
  }

  /// @name     size
  /// @brief    获取文件大小
  ///
  /// @param    NONE
  ///
  /// @return   NONE
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:06:30
  /// @warning  线程不安全
  inline size_t size() const {
    if (fd_ == nullptr) {
      throw("Cannot use size() on closed file " + (filename_));
    }
    return filesize(fd_);
  }

  const std::string &filename() const { return filename_; }

  //
  // return file path and its extension:
  //
  /// @name     split_by_extension
  /// @brief    分割文件名称成文件路径加名字与其扩展名
  /// @details
  ///          "mylog.txt" => ("mylog", ".txt")
  ///          "mylog" => ("mylog", "")
  ///          "mylog." => ("mylog.", "")
  ///          "/dir1/dir2/mylog.txt" => ("/dir1/dir2/mylog", ".txt")
  //
  ///          the starting dot in filenames is ignored (hidden files):
  //
  ///          ".mylog" => (".mylog". "")
  ///          "my_folder/.mylog" => ("my_folder/.mylog", "")
  ///          "my_folder/.mylog.txt" => ("my_folder/.mylog", ".txt")
  ///
  /// @param    fname [in]  文件名称
  ///
  /// @return   分割后的文件名和扩展名
  ///
  /// @author   Lijiancong, pipinstall@163.com
  /// @date     2020-07-18 16:09:39
  /// @warning  线程不安全
  static std::tuple<std::string, std::string> split_by_extension(
      const std::string &fname) {
    auto ext_index = fname.rfind('.');

    // no valid extension found - return whole path and empty string as
    // extension
    if (ext_index == std::string::npos || ext_index == 0 ||
        ext_index == fname.size() - 1) {
      return std::make_tuple(fname, std::string());
    }

    // treat cases like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
    auto folder_index = fname.rfind(folder_sep);
    if (folder_index != std::string::npos && folder_index >= ext_index - 1) {
      return std::make_tuple(fname, std::string());
    }

    // finally - return a valid base and extension tuple
    return std::make_tuple(fname.substr(0, ext_index), fname.substr(ext_index));
  }

 private:
  const int open_tries_ = 5;
  const int open_interval_ = 10;
  std::FILE *fd_{nullptr};
  std::string filename_;
};
}  // namespace file_helper
}  // namespace lee

#endif  // end of MY_LOG_INCLUDE_FILE_HELPER_H_
