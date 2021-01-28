
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////
/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.
///
/// @file   lazy_string.hpp
/// @brief  惰性字符串的拼接
///
/// @author lijiancong, pipinstall@163.com
/// @date   2020-07-22 20:35:21
///////// ///////// ///////// ///////// ///////// ///////// ///////// /////////

#ifndef INCLUDE_MY_LOG_LAZY_STRING_HPP_
#define INCLUDE_MY_LOG_LAZY_STRING_HPP_

#include <algorithm>
#include <string>

namespace lee {
inline namespace string {
template <typename... Strings>
class lazy_string_concat_helper;

template <typename last_str, typename... str>
class lazy_string_concat_helper<last_str, str...> {
 public:
  lazy_string_concat_helper(last_str data,
                            lazy_string_concat_helper<str...> tail)
      : data_(data), tail_(tail) {}
  last_str data_;
  lazy_string_concat_helper<str...> tail_;
  int size() const { return static_cast<int>(data_.size() + tail_.size()); }

  template <typename It>
  void save(It end) const {
    const auto begin = end - data_.size();
    std::copy(data_.cbegin(), data_.cend(), begin);
    tail_.save(begin);
  }

  operator std::string() const {
    std::string result(size(), '\0');
    save(result.end());
    return result;
  }

  lazy_string_concat_helper<std::string, last_str, str...> operator+(
      const std::string &other) const {
    return lazy_string_concat_helper<std::string, last_str, str...>(other,
                                                                    *this);
  }
};

/// 特化一个中止条件对象
template <>
class lazy_string_concat_helper<> {
 public:
  lazy_string_concat_helper() {}
  int size() const { return 0; }
  template <typename It>
  void save(It) const {}

  lazy_string_concat_helper<std::string> operator+(
      const std::string &other) const {
    return lazy_string_concat_helper<std::string>(other, *this);
  }
};
}  // namespace string
}  // namespace lee

#endif
