/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#include "profiler.hpp"

#include <catch2/catch.hpp>

TEST_CASE("profiler", "log") {
  for (auto i = 0; i < 10; i++) {
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
    for (auto x = 0; x < 1000; x++) {
      PROFILER_F();
    }
  }
}
