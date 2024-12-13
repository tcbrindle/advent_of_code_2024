
#include <aoc.hpp>

#include <ctre.hpp>

namespace {

using i64 = std::int64_t;

using vec2 = aoc::vec2_t<i64>;

struct game_info {
    vec2 a;
    vec2 b;
    vec2 prize;
};

constexpr auto& game_regex =
    R"(Button A: X\+(\d+), Y\+(\d+)
Button B: X\+(\d+), Y\+(\d+)
Prize: X=(\d+), Y=(\d+)\n?)";

auto const parse_input = [](std::string_view input) -> std::vector<game_info> {
    return flux::split_string(input, "\n\n")
        .map([](std::string_view game) {
            auto [_, ax, ay, bx, by, px, py] = ctre::match<game_regex>(game);
            return game_info{.a = {ax.to_number(), ay.to_number()},
                             .b = {bx.to_number(), by.to_number()},
                             .prize = {px.to_number(), py.to_number()}};
        })
        .to<std::vector>();
};

auto const solve = [](game_info const& game) -> std::optional<i64> {
    auto [a, b, prize] = game;

    i64 i = b.y * prize.x - b.x * prize.y;
    i64 j = -a.y * prize.x + a.x * prize.y;
    i64 det = (a.x * b.y) - (a.y * b.x);

    if (det == 0 || i % det != 0 || j % det != 0) {
        return std::nullopt;
    } else {
        return 3 * i / det + j / det;
    }
};

auto const part1 = [](std::vector<game_info> const& games) -> i64 {
    return flux::ref(games).map(solve).filter_deref().sum();
};

auto const part2 = [](std::vector<game_info> const& games) -> i64 {
    return flux::ref(games)
        .map([](game_info g) {
            g.prize += vec2{10000000000000, 10000000000000};
            return solve(g);
        })
        .filter_deref()
        .sum();
};

constexpr auto& test_input =
    R"(Button A: X+94, Y+34
Button B: X+22, Y+67
Prize: X=8400, Y=5400

Button A: X+26, Y+66
Button B: X+67, Y+21
Prize: X=12748, Y=12176

Button A: X+17, Y+86
Button B: X+84, Y+37
Prize: X=7870, Y=6450

Button A: X+69, Y+23
Button B: X+27, Y+71
Prize: X=18641, Y=10279
)";

static_assert(part1(parse_input(test_input)) == 480);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const games = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(games));
    std::println("Part 2: {}", part2(games));
}