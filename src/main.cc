/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

/// #define CATCH_CONFIG_RUNNER
/// #include <catch2/catch.hpp>
#include <iostream>

#include "log_wrapper.hpp"
#include "profiler.hpp"

int main(int argc, char** argv) {
  /// Catch::Session().run(argc, argv);
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
  return 0;
}
