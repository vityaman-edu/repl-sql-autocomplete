#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace repl::test {

auto factorial(uint32_t number) -> uint32_t {
  return number <= 1 ? number : factorial(number - 1) * number;
}

namespace {
// NOLINTBEGIN

TEST_CASE("Factorials are computed", "[factorial]") {
  REQUIRE(factorial(1) == 1);
  REQUIRE(factorial(2) == 2);
  REQUIRE(factorial(3) == 6);
  REQUIRE(factorial(10) == 3'628'800);
}

// NOLINTEND
} // namespace

} // namespace repl::test
