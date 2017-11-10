#include <vector>
#include <iostream>
#include <exception>
#include <stdexcept>

#include "foxy/expected.hpp"

#include <catch.hpp>

TEST_CASE("Our Expected type")
{
  using expected_t = foxy::expected<int, std::exception>;

  SECTION("should be default-constructible for a given type")
  {
    expected_t expected{};
    REQUIRE(expected.is_valid());
  }

  SECTION("should be copy-constructible")
  {
    auto const expected  = expected_t{};
    auto const expected2 = expected_t{expected};
    REQUIRE(expected.is_valid());
    REQUIRE(expected2.is_valid());
  }

  SECTION("should be parameter-constructible")
  {
    auto const expected = expected_t{1337};
    REQUIRE(expected.is_valid());
  }

  SECTION("should should support moving parameters in its constructor")
  {
    auto v = std::vector<int>{1, 2, 3, 4};

    auto const expected =
      foxy::expected<
        std::vector<int>,
        std::exception
      >{std::move(v)};

    REQUIRE(expected.is_valid());
  }

  SECTION("should support fmap to some degree")
  {
    using foxy::$;

    auto expected = expected_t{1337};

    auto const fmap = foxy::fmap{};

    expected = fmap(
      [](int const x) -> int { return x + 1; },
      expected);

    auto const f = [](int const x) -> int
    {
      std::cout << x << "\n";
      return x;
    };

    expected = f <$> expected;

    REQUIRE(expected.is_valid());

    auto const thrower = [](auto&& x)
    {
      throw std::logic_error{"bad times, man; bad times"};
      return x;
    };

    expected = thrower <$> expected;

    REQUIRE(!expected.is_valid());
  }
}