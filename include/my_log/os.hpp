///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   os.hpp
/// @brief  跟系统相关的工具
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-18 15:31:14
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef INCLUDE_MY_LOG_OS_HPP_
#define INCLUDE_MY_LOG_OS_HPP_

#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <thread>

#ifdef _WIN32

#include <io.h>       // _get_osfhandle and _isatty support
#include <process.h>  //  _get_pid support
#ifndef NOMINMAX
#define NOMINMAX  // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef __MINGW32__
#include <share.h>
#endif

#include <direct.h>  // for _mkdir/_wmkdir

#else  // unix

#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h>  //Use gettid() syscall under linux to get thread id

#elif defined(_AIX)
#include <pthread.h>  // for pthread_getthreadid_np

#elif defined(__DragonFly__) || defined(__FreeBSD__)
#include <pthread_np.h>  // for pthread_getthreadid_np

#elif defined(__NetBSD__)
#include <lwp.h>  // for _lwp_self

#elif defined(__sun)
#include <thread.h>  // for thr_self
#endif

#endif  // unix

#ifndef __has_feature       // Clang - feature checking macros.
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

namespace lee {
inline namespace os {
// folder separator
#ifdef _WIN32
static const char folder_sep = '\\';
#else
constexpr static const char folder_sep = '/';
#endif

/// @name     get_time_string
/// @brief    获取毫秒级别的格式化时间
///
/// @param    NONE
///
/// @return   格式化后的时间
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-19 09:31:00
/// @warning  线程不安全
inline std::string get_time_string() {
  auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();

  tm buf;
  auto t1 = t / 1000;
#ifdef _WIN32
  localtime_s(&buf, &t1);
#else
  localtime_r(&t1, &buf);
#endif
  char p[32] = {0};
  strftime(p, sizeof(p), "[%F %T", &buf);
  auto time_str = std::to_string(t % 1000);
  while (time_str.size() < 3) {
    time_str = "0" + time_str;
  }
  return std::string(p) + "." +  + "]";
}

/// @name     path_exists
/// @brief    判断一个路径或文件是否存在
///
/// @param    filename  [in]  路径名称
///
/// @return   存在则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:48:00
/// @warning  线程不安全
inline bool path_exists(const std::string &filename) noexcept {
#ifdef _WIN32
  auto attribs = ::GetFileAttributesA(filename.c_str());
  return attribs != ((DWORD)-1);
#else  // common linux/unix all have the stat system call
  struct stat buffer;
  return (::stat(filename.c_str(), &buffer) == 0);
#endif
}

/// @name     mkdir_
/// @brief    创建一个路径
///
/// @param    path  [in]  要创建的路径
///
/// @return   创建成功则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:45:31
/// @warning  线程不安全
static inline bool mkdir_(const std::string &path) {
#ifdef _WIN32
  return ::_mkdir(path.c_str()) == 0;
#else
  return ::mkdir(path.c_str(), mode_t(0755)) == 0;
#endif
}

// create the given directory - and all directories leading to it
// return true on success or if the directory already exists
/// @name     create_dir
/// @brief    创建一个路径
///
/// @param    path [in] 要创建的路径
///
/// @return   创建成功或路径已经存在则返回真
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:42:57
/// @warning  线程不安全
inline bool create_dir(std::string path) {
  if (lee::path_exists(path)) {
    return true;
  }

  if (path.empty()) {
    return false;
  }

#ifdef _WIN32
  // support forward slash in windows
  std::replace(path.begin(), path.end(), '/', folder_sep);
#endif

  size_t search_offset = 0;
  do {
    auto token_pos = path.find(folder_sep, search_offset);
    // treat the entire path as a folder if no folder separator not found
    if (token_pos == std::string::npos) {
      token_pos = path.size();
    }

    auto subdir = path.substr(0, token_pos);

    if (!subdir.empty() && !path_exists(subdir) && !mkdir_(subdir)) {
      return false;  // return error if failed creating dir
    }
    search_offset = token_pos + 1;
  } while (search_offset < path.size());

  return true;
}

/// @name     dir_name
/// @brief    获取文件夹名称
/// @details  Return directory name from given path or empty string
///           "abc/file" => "abc"
///           "abc/" => "abc"
///           "abc" => ""
///           "abc///" => "abc//"
///
/// @param    path [in] 完整路径名
///
/// @return   文件夹名称
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:56:13
/// @warning  线程不安全
inline std::string dir_name(std::string path) {
#ifdef _WIN32
  // support forward slash in windows
  std::replace(path.begin(), path.end(), '/', folder_sep);
#endif
  auto pos = path.find_last_of(folder_sep);
  return pos != std::string::npos ? path.substr(0, pos) : std::string{};
}

/// @name     sleep_for_millis
/// @brief    本线程睡眠一定毫秒
///
/// @param    milliseconds  [in]  毫秒数
///
/// @return   NONE
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 15:59:22
/// @warning  线程不安全
inline void sleep_for_millis(int milliseconds) noexcept {
#if defined(_WIN32)
  ::Sleep(milliseconds);
#else
  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#endif
}

/// @name     filesize
/// @brief    返回一个文件的大小
///
/// @param    f [in]  文件指针
///
/// @return   文件大小
///
/// @author   Lijiancong, pipinstall@163.com
/// @date     2020-07-18 16:07:56
/// @warning  线程不安全
inline size_t filesize(FILE *f) {
  if (f == nullptr) {
    throw("Failed getting file size. fd is null");
  }
#if defined(_WIN32) && !defined(__CYGWIN__)
  int fd = ::_fileno(f);
#if _WIN64  // 64 bits
  __int64 ret = ::_filelengthi64(fd);
  if (ret >= 0) {
    return static_cast<size_t>(ret);
  }

#else  // windows 32 bits
  int16 ret = ::_filelength(fd);
  if (ret >= 0) {
    return static_cast<size_t>(ret);
  }
#endif

#else  // unix
// OpenBSD doesn't compile with :: before the fileno(..)
#if defined(__OpenBSD__)
  int fd = fileno(f);
#else
  int fd = ::fileno(f);
#endif
// 64 bits(but not in osx or cygwin, where fstat64 is deprecated)
#if (defined(__linux__) || defined(__sun) || defined(_AIX)) && \
    (defined(__LP64__) || defined(_LP64))
  struct stat64 st;
  if (::fstat64(fd, &st) == 0) {
    return static_cast<size_t>(st.st_size);
  }
#else  // other unix or linux 32 bits or cygwin
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    return static_cast<size_t>(st.st_size);
  }
#endif
#endif
  throw("Failed getting file size from fd");
  /// return 0;  // will not be reached.
}

inline int remove(const std::string &filename) noexcept {
  return std::remove(filename.c_str());
}

inline int remove_if_exists(const std::string &filename) noexcept {
  return path_exists(filename) ? remove(filename) : 0;
}

inline int rename(const std::string &filename1,
                  const std::string &filename2) noexcept {
  return std::rename(filename1.c_str(), filename2.c_str());
}

// fopen_s on non windows for writing
inline bool fopen_s(FILE **fp, const std::string &filename,
                    const std::string &mode) {
#ifdef _WIN32
#ifdef SPDLOG_WCHAR_FILENAMES
  *fp = ::_wfsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#else
  *fp = ::_fsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#endif
#if defined(SPDLOG_PREVENT_CHILD_FD)
  if (*fp != nullptr) {
    auto file_handle = reinterpret_cast<HANDLE>(_get_osfhandle(::_fileno(*fp)));
    if (!::SetHandleInformation(file_handle, HANDLE_FLAG_INHERIT, 0)) {
      ::fclose(*fp);
      *fp = nullptr;
    }
  }
#endif
#else  // unix
  *fp = ::fopen((filename.c_str()), mode.c_str());
#endif

  return *fp == nullptr;
}
}  // namespace os
}  // namespace lee

#endif  // INCLUDE_MY_LOG_OS_HPP_
