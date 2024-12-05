
#include <aoc.hpp>

#include <ankerl/unordered_dense.h>

namespace {

using rules_t = ankerl::unordered_dense::set<std::pair<int, int>>;

using update_t = std::vector<int>;

auto const parse_rules = [](std::string_view input) -> rules_t {
    return flux::split_string(input, '\n')
        .map([](std::string_view line) {
            auto bar = line.find('|');
            return std::pair(aoc::parse<int>(line.substr(0, bar)),
                             aoc::parse<int>(line.substr(bar + 1)));
        })
        .to<rules_t>();
};

auto const parse_updates = [](std::string_view input) -> std::vector<update_t> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            return flux::split_string(line, ',')
                .map(aoc::parse<int>)
                .to<update_t>();
        })
        .to<std::vector>();
};

auto const parse_input
    = [](std::string_view input) -> std::pair<rules_t, std::vector<update_t>> {
    auto blank = input.find("\n\n");
    return std::pair(parse_rules(input.substr(0, blank)),
                     parse_updates(input.substr(blank + 2)));
};

auto const part1
    = [](rules_t const& rules, std::vector<update_t> const& updates) -> int {
    return flux::ref(updates)
        .filter([&rules](update_t const& u) {
            return std::ranges::is_sorted(
                u, [&rules](int l, int r) { return rules.contains({l, r}); });
        })
        .map([](update_t const& u) { return u.at(u.size() / 2); })
        .sum();
};

auto const part2
    = [](rules_t const& rules, std::vector<update_t> const& updates) -> int {
    auto cmp = [&](int l, int r) { return rules.contains({l, r}); };
    return flux::ref(updates)
        .filter(
            [&](update_t const& u) { return !std::ranges::is_sorted(u, cmp); })
        .map([&](update_t u) {
            std::ranges::sort(u, cmp);
            return u.at(u.size() / 2);
        })
        .sum();
};

constexpr auto& test_input =
    R"(47|53
97|13
97|61
97|47
75|29
61|13
75|53
29|13
97|29
53|29
61|53
97|53
61|29
47|13
75|47
97|75
47|61
75|61
47|29
75|13
53|13

75,47,61,53,29
97,61,53,29,13
75,29,13
75,97,47,61,53
61,13,29
97,13,75,29,47
)";

} // namespace

int main(int argc, char** argv)
{
    // Tests
    {
        auto [rules, updates] = parse_input(test_input);
        assert(part1(rules, updates) == 143);
        assert(part2(rules, updates) == 123);
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto [rules, updates] = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(rules, updates));
    std::println("Part 2: {}", part2(rules, updates));
}