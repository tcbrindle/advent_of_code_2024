
#include <aoc.hpp>

namespace {

using i64 = std::int64_t;

using equation = std::pair<i64, std::vector<i64>>;

auto parse_input = [](std::string_view input) -> std::vector<equation> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            auto colon = line.find(':');
            return equation{aoc::parse<i64>(line.substr(0, colon)),
                            flux::split_string(line.substr(colon + 2), ' ')
                                .map(aoc::parse<i64>)
                                .to<std::vector>()};
        })
        .to<std::vector>();
};

auto const concat = [](i64 a, i64 b) -> i64 {
    auto tmp = b;
    while (tmp > 0) {
        a *= 10;
        tmp /= 10;
    }
    return a + b;
};

template <bool Part2>
auto const is_valid = [](equation const& eq) -> bool {
    auto const& [target, args] = eq;

    std::vector<std::pair<i64, std::size_t>> stack;
    stack.push_back({args.at(0), 1});

    while (!stack.empty()) {
        auto [so_far, idx] = stack.back();
        stack.pop_back();
        if (so_far > target) {
            continue;
        }

        i64 val = args.at(idx);
        if (idx == args.size() - 1) {
            bool valid = (so_far + val == target) || (so_far * val == target);
            if constexpr (Part2) {
                valid |= (concat(so_far, val) == target);
            }
            if (valid) {
                return true;
            }
        } else {
            stack.push_back({so_far + val, idx + 1});
            stack.push_back({so_far * val, idx + 1});
            if constexpr (Part2) {
                stack.push_back({concat(so_far, val), idx + 1});
            }
        }
    }
    return false;
};

template <bool Part2>
auto calculate = [](std::vector<equation> const& eqs) -> i64 {
    return flux::ref(eqs).filter(is_valid<Part2>).map(&equation::first).sum();
};

auto part1 = calculate<false>;
auto part2 = calculate<true>;

constexpr auto& test_input =
    R"(190: 10 19
3267: 81 40 27
83: 17 5
156: 15 6
7290: 6 8 6 15
161011: 16 10 13
192: 17 8 14
21037: 9 7 18 13
292: 11 6 16 20
)";

static_assert([] {
    auto const equations = parse_input(test_input);
    return part1(equations) == 3749 && part2(equations) == 11387;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const equations = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(equations));
    std::println("Part 2: {}", part2(equations));
}