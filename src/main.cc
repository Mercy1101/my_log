#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <iostream>

int main(int argc, char** argv) {
  Catch::Session().run(argc, argv);

  return 0;
}