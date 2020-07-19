/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#include "log_wrapper.hpp"
#include "profiler.hpp"

#include <catch2/catch.hpp>

TEST_CASE("log_wrapper", "log") {
  LOG_TRACE("trace this is a test");
  LOG_DEBUG("debug this is a test");
  LOG_INFO("info this is a test");
  LOG_WARN("warn this is a test");
  LOG_ERROR("error this is a test");
}

TEST_CASE("log_wrapper2", "log2") {
  for (auto i = 0; i < 10; i++) {
    PROFILER_F();
    for (auto x = 0; x < 1000; x++) {
      LOG_TRACE("trace " + std::to_string(x));
    }
    for (auto x = 0; x < 1000; x++) {
      LOG_DEBUG("debug " + std::to_string(x));
    }
    for (auto x = 0; x < 1000; x++) {
      LOG_INFO("info " + std::to_string(x));
    }
    for (auto x = 0; x < 1000; x++) {
      LOG_WARN("warn " + std::to_string(x));
    }
    for (auto x = 0; x < 1000; x++) {
      LOG_ERROR("error " + std::to_string(x));
    }
    for (auto x = 0; x < 1000; x++) {
      LOG_CRITICAL("critical " + std::to_string(x));
    }
  }
}
