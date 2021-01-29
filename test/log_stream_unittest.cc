/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#include "log_stream.hpp"

#include <catch2/catch.hpp>

#include "profiler.hpp"

TEST_CASE("log_stream 简单测试", "log") {
  LOG(TRACE) << "string"
             << " " << 33 << " " << 55.0;
  LOG(DEBUG) << "string"
             << " " << 33 << " " << 55.0;
  LOG(INFO) << "string"
            << " " << 33 << " " << 55.0;
  LOG(WARN) << "string"
            << " " << 33 << " " << 55.0;
  LOG(ERROR) << "string"
             << " " << 33 << " " << 55.0;
  LOG(CRITICAL) << "string"
                << " " << 33 << " " << 55.0;
}

TEST_CASE("log_stream效能测试", "log2") {
  for (auto i = 0; i < 10; i++) {
    PROFILER_F();
    for (auto x = 0; x < 1000; x++) {
      LOG(TRACE) << "string"
                 << " " << 33 << " " << 55.0;
      LOG(DEBUG) << "string"
                 << " " << 33 << " " << 55.0;
      LOG(INFO) << "string"
                << " " << 33 << " " << 55.0;
      LOG(WARN) << "string"
                << " " << 33 << " " << 55.0;
      LOG(ERROR) << "string"
                 << " " << 33 << " " << 55.0;
      LOG(CRITICAL) << "string"
                    << " " << 33 << " " << 55.0;
    }
  }
}

TEST_CASE("log_stream 特殊值测试", "log3") {
  LOG(TRACE) << ("string");
  LOG(DEBUG) << ("string");
  LOG(INFO) << ("string");
  LOG(WARN) << ("string");
  LOG(ERROR) << ("string");
  LOG(CRITICAL) << ("string");
}
