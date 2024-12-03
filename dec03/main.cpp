
#include <aoc.hpp>

#include <ctre.hpp>

namespace {

auto part1 = [](std::string_view input) -> int {
    constexpr auto& regex = R"(mul\((\d{1,3}),(\d{1,3})\))";

    return flux::from_range(ctre::search_all<regex>(input))
        .map([](auto result) {
            auto [_, a, b] = result;
            return a.to_number() * b.to_number();
        })
        .sum();
};

auto part2 = [](std::string_view input) -> int {
    return flux::split_string(input, "do()")
        .map([](std::string_view chunk) {
            return flux::split_string(chunk, "don't()")
                .front()
                .map(part1)
                .value();
        })
        .sum();
};

constexpr auto& test_input1
    = R"(xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5)))";

constexpr auto& test_input2
    = R"(xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";

static_assert(part1(test_input1) == 161);
static_assert(part2(test_input2) == 48);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}