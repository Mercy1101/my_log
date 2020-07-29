/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#include "my_log/lazy_string.hpp"

#include <catch2/catch.hpp>

#include "profiler.hpp"

TEST_CASE("lazy_string", "[my_log][lazy_string]") {
  {
    PROFILER_F();
    for (int i = 0; i < 10000; ++i) {
      lee::lazy_string_concat_helper<> lazy_concat;
      std::string str_log =
          lazy_concat + lee::get_time_string() + " " + std::to_string(i) + " " +
          " <In Function: " + std::to_string(0.0 + i) + "," +
          ", File: " + "lksjdflksdjflkdsjlkdsfjldskfjsdlkfjslkdfjlksddjfsl" +
          " Line: " + std::to_string(__LINE__) + ", PID: " + __FILE__ + ">\n";
      std::cout << str_log << std::endl;
    }
  }
}

TEST_CASE("string", "[my_log][lazy_string]") {
  {
    PROFILER_F();
    for (int i = 0; i < 10000; ++i) {
      std::string lazy_concat;
      std::string str_log =
          lazy_concat + lee::get_time_string() + " " + std::to_string(i) + " " +
          " <In Function: " + std::to_string(0.0 + i) + "," +
          ", File: " + "lksjdflksdjflkdsjlkdsfjldskfjsdlkfjslkdfjlksddjfsl" +
          " Line: " + std::to_string(__LINE__) + ", PID: " + __FILE__ + ">\n";
      std::cout << str_log << std::endl;
    }
  }
}
