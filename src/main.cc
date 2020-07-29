/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <iostream>

#include "log_wrapper.hpp"
#include "profiler.hpp"

int main(int argc, char** argv) {
  Catch::Session().run(argc, argv);
  return 0;
}
