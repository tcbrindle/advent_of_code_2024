
#include <aoc.hpp>

#include <ankerl/unordered_dense.h>

namespace {

using u64 = std::uint64_t;

using stones_map = ankerl::unordered_dense::map<u64, u64>;

auto const parse_input = [](std::string_view str) -> stones_map {
    stones_map m;
    flux::split_string(str, ' ').map(aoc::parse<u64>).for_each([&m](u64 i) {
        ++m[i];
    });
    return m;
};

auto const count_digits = [](u64 n) -> int {
    int i = 0;
    while (n > 0) {
        n /= 10;
        ++i;
    }
    return i;
};

// Returns nullopt if n has an odd number of digits
auto const split_digits = [](u64 n) -> std::optional<std::pair<u64, u64>> {
    int n_digits = count_digits(n);
    if (n_digits % 2 != 0) {
        return std::nullopt;
    }

    u64 first = n;
    int k = 1;
    for (auto _ : flux::ints(0, n_digits / 2)) {
        first /= 10;
        k *= 10;
    }
    return std::pair{first, n - (first * k)};
};

template <int N>
auto blink = [](stones_map stones) -> u64 {
    stones_map next;

    for (auto _ : flux::ints(0, N)) {
        for (auto [val, count] : stones) {
            if (val == 0) {
                next[1] += count;
            } else if (auto opt = split_digits(val)) {
                next[opt->first] += count;
                next[opt->second] += count;
            } else {
                next[val * 2024] += count;
            }
        }
        std::swap(stones, next);
        next.clear();
    }

    return flux::sum(stones | std::views::values);
};

auto const part1 = blink<25>;
auto const part2 = blink<75>;

[[maybe_unused]] constexpr auto& test_input = "125 17";

} // namespace

int main(int argc, char** argv)
{
    {
        assert(part1(parse_input(test_input)) == 55312);
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const stones = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(stones));
    std::println("Part 2: {}", aoc::timed(part2, stones));
}