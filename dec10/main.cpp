
#include <aoc.hpp>

namespace {

struct position {
    int x = 0;
    int y = 0;

    friend constexpr auto operator==(position const&, position const&) -> bool
        = default;

    friend constexpr auto operator<=>(position const&, position const&)
        = default;
};

struct grid2d {
    std::string data;
    int width;
    int height;

    constexpr auto is_in_bounds(position p) const -> bool
    {
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }

    constexpr auto operator[](position p) const -> char
    {
        return data.at(p.y * width + p.x);
    }

    constexpr auto positions() const -> flux::random_access_sequence auto
    {
        return flux::cartesian_product_map(
            [](int j, int i) { return position{i, j}; }, flux::iota(0, height),
            flux::iota(0, width));
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .width = int(flux::find(input, '\n')),
        .height = int(flux::count_eq(input, '\n'))};
};

auto const get_neighbours = [](position p) {
    return flux::from(std::array{position{p.x - 1, p.y}, position{p.x + 1, p.y},
                                 position{p.x, p.y - 1},
                                 position{p.x, p.y + 1}});
};

struct trail_info {
    int score;
    int rating;
};

auto const walk_trail = [](grid2d const& grid, position start) -> trail_info {
    std::vector<position> goals;
    goals.reserve(grid.data.size() / 10);

    [&](this auto const& self, position here) -> void {
        char value = grid[here];
        if (value == '9') {
            goals.push_back(here);
        } else {
            get_neighbours(here)
                .filter([&](position p) {
                    return grid.is_in_bounds(p) && (grid[p] == value + 1);
                })
                .for_each(self);
        }
    }(start);

    flux::sort(goals);
    int score = flux::ref(goals).dedup().count();

    return {.score = score, .rating = int(goals.size())};
};

auto const walk_all = [](std::string_view input) -> trail_info {
    auto grid = parse_input(input);
    return grid.positions()
        .filter([&](auto pos) { return grid[pos] == '0'; })
        .map(std::bind_front(walk_trail, grid))
        .fold(
            [](auto sum, auto info) {
                sum.score += info.score;
                sum.rating += info.rating;
                return sum;
            },
            trail_info{});
};

/*
 * Part 1 tests (lots of them today)
 */

constexpr auto& test_input1 = R"(0123
1234
8765
9876
)";
static_assert(walk_all(test_input1).score == 1);

constexpr auto& test_input2 = R"(...0...
...1...
...2...
6543456
7.....7
8.....8
9.....9
)";
static_assert(walk_all(test_input2).score == 2);

constexpr auto& test_input3 =
    R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....
)";
static_assert(walk_all(test_input3).score == 4);

constexpr auto& test_input4 = R"(10..9..
2...8..
3...7..
4567654
...8..3
...9..2
.....01
)";
static_assert(walk_all(test_input4).score == 3);

constexpr auto& test_input5 =
    R"(89010123
78121874
87430965
96549874
45678903
32019012
01329801
10456732
)";
static_assert(walk_all(test_input5).score == 36);

/*
 * Part 2 tests
 */
constexpr auto& test_input6 = R"(.....0.
..4321.
..5..2.
..6543.
..7..4.
..8765.
..9....
)";
static_assert(walk_all(test_input6).rating == 3);

constexpr auto& test_input7 = R"(..90..9
...1.98
...2..7
6543456
765.987
876....
987....
)";
static_assert(walk_all(test_input7).rating == 13);

constexpr auto& test_input8 = R"(012345
123456
234567
345678
4.6789
56789.
)";
static_assert(walk_all(test_input8).rating == 227);

constexpr auto& test_input9 = test_input5;
static_assert(walk_all(test_input9).rating == 81);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto [info, time] = aoc::timed(walk_all, aoc::string_from_file(argv[1]));

    std::println("{}", time);
    std::println("Part 1: {}", info.score);
    std::println("Part 2: {}", info.rating);
}