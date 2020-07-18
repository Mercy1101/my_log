/// Copyright (c) 2019,2020 Lijiancong. All rights reserved.
///
/// Use of this source code is governed by a MIT license
/// that can be found in the License file.

#include <catch2/catch.hpp>

using namespace lee;

void prepare_logdir()
{
#ifdef _WIN32
    system("rmdir /S /Q test_logs");
#else
    auto rv = system("rm -rf test_logs");
    if (rv != 0)
    {
        throw std::runtime_error("Failed to rm -rf test_logs");
    }
#endif
}

TEST_CASE("file_helper_filename", "[file_helper::filename()]]")
{
    prepare_logdir();

    file_helper helper;
    std::string target_filename = "test_logs/file_helper_test.txt";
    helper.open(target_filename);
    REQUIRE(helper.filename() == target_filename);
}