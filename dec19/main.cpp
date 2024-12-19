#include <aoc.hpp>

namespace {

using i64 = std::int64_t;

auto const parse_input = [](std::string_view input)
    -> std::pair<std::vector<std::string>, std::vector<std::string>> {
    auto split = input.find("\n\n");

    auto patterns = flux::split_string(input.substr(0, split), ", ")
                        .to<std::vector<std::string>>();

    auto designs
        = flux::split_string(input.substr(split + 2), '\n')
              .filter([](std::string_view line) { return !line.empty(); })
              .to<std::vector<std::string>>();

    return std::pair(std::move(patterns), std::move(designs));
};

auto part1 = [](std::span<std::string const> patterns,
                std::span<std::string const> designs) -> i64 {
    auto test_design
        = [&](this auto const& self, std::string_view design) -> bool {
        if (design.empty()) {
            return true;
        }

        return flux::any(patterns, [&](std::string_view pattern) {
            return design.starts_with(pattern)
                && self(design.substr(pattern.size()));
        });
    };

    return flux::count_if(designs, test_design);
};

auto part2 = [](std::span<std::string const> patterns,
                std::span<std::string const> designs) -> i64 {
    aoc::hash_map<std::string_view, i64> memo;

    auto count_designs
        = [&](this auto const& self, std::string_view design) -> i64 {
        if (design.empty()) {
            return 1;
        }

        if (auto iter = memo.find(design); iter != memo.cend()) {
            return iter->second;
        }

        i64 count = flux::ref(patterns)
                        .filter([&](std::string_view pattern) {
                            return flux::starts_with(design, pattern);
                        })
                        .map([&](std::string_view pattern) {
                            return design.substr(pattern.size());
                        })
                        .map(self)
                        .sum();

        memo[design] = count;
        return count;
    };

    return flux::map(designs, count_designs).sum();
};

constexpr auto& test_input =
    R"(r, wr, b, g, bwu, rb, gb, br

brwrr
bggr
gbbr
rrbgbr
ubwu
bwurrg
brgr
bbrgwb
)";

} // namespace

int main(int argc, char** argv)
{
    {
        [[maybe_unused]] auto const [patterns, designs]
            = parse_input(test_input);
        assert(part1(patterns, designs) == 6);
        assert(part2(patterns, designs) == 16);
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const [patterns, designs]
        = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(patterns, designs));
    std::println("Part 2: {}", aoc::timed(part2, patterns, designs));
}