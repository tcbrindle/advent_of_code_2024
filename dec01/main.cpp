
#include <aoc.hpp>

namespace {

using i64 = std::int64_t;

// std::abs is not constexpr
const auto abs = [](i64 a) { return a < 0 ? -a : a; };

const auto parse_input = [](std::string_view input)
    -> std::pair<std::vector<i64>, std::vector<i64>> {
    return flux::split_string(input, "\n")
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            auto space = line.find(' ', 1);
            return std::pair(aoc::parse<i64>(line.substr(0, space)),
                             aoc::parse<i64>(line.substr(space)));
        })
        .fold(
            [](auto vec_pair, auto int_pair) {
                vec_pair.first.push_back(int_pair.first);
                vec_pair.second.push_back(int_pair.second);
                return vec_pair;
            },
            std::pair<std::vector<i64>, std::vector<i64>>{});
};

auto part1 = [](std::vector<i64> vec1, std::vector<i64> vec2) -> i64 {
    flux::sort(vec1);
    flux::sort(vec2);
    return flux::zip_fold(
        [](i64 sum, i64 a, i64 b) { return sum + abs(a - b); }, 0, vec1, vec2);
};

auto part2
    = [](std::vector<i64> const& vec1, std::vector<i64> const& vec2) -> i64 {
    return flux::ref(vec1)
        .map([&vec2](i64 elem) { return elem * flux::count_eq(vec2, elem); })
        .sum();
};

constexpr auto& test_data =
    R"(3   4
4   3
2   5
1   3
3   9
3   3)";

static_assert([] {
    auto [v1, v2] = parse_input(test_data);
    return part1(v1, v2) == 11 && part2(v1, v2) == 31;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto [vec1, vec2] = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(vec1, vec2));
    fmt::println("Part 2: {}", part2(vec1, vec2));
}