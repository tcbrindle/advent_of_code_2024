
#include <aoc.hpp>

namespace {

using vec2 = aoc::vec2_t<int>;

struct grid2d {
    std::string data;
    int width;
    int height;

    constexpr auto operator[](vec2 const& p) const -> char
    {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height) {
            return '.';
        } else {
            return data.at(p.y * width + p.x);
        }
    }

    constexpr auto positions() const -> flux::random_access_sequence auto
    {
        return flux::cartesian_product_map(
            [](int j, int i) { return vec2{i, j}; }, flux::iota(0, height),
            flux::iota(0, width));
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .width = int(flux::find(input, '\n')),
        .height = int(flux::count_eq(input, '\n'))};
};

constexpr auto neighbour_offsets
    = std::array<vec2, 4>{vec2{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

constexpr auto corner_offsets = std::array<std::array<vec2, 3>, 4>{
    std::array<vec2, 3>{vec2{-1, -1}, {-1, 0}, {0, -1}},
    std::array<vec2, 3>{vec2{1, -1}, {1, 0}, {0, -1}},
    std::array<vec2, 3>{vec2{1, 1}, {1, 0}, {0, 1}},
    std::array<vec2, 3>{vec2{-1, 1}, {-1, 0}, {0, 1}}};

struct prices {
    int part1;
    int part2;
};

auto const calculate_prices = [](std::string_view input) -> prices {
    grid2d grid = parse_input(input);

    auto unassigned_positions = grid.positions().to<aoc::hash_set>();

    auto build_region
        = [&](this const auto& self, vec2 pos) -> std::tuple<int, int, int> {
        unassigned_positions.erase(pos);
        int area = 1;
        int perimeter = 4;
        int corners = 0;
        char val = grid[pos];

        for (auto const& neighbours : corner_offsets) {
            int count = flux::count_if(
                neighbours, [&](vec2 n) { return grid[pos + n] == val; });

            if (count == 0) {
                corners += 3;
            } else if (count == 1 && grid[pos + neighbours[0]] == val) {
                corners += 3;
            } else if (count == 2) {
                ++corners;
            }
        }

        for (vec2 offset : neighbour_offsets) {
            vec2 next = pos + offset;
            if (grid[next] == val) {
                --perimeter;
                if (unassigned_positions.contains(next)) {
                    auto [next_area, next_perim, next_corners] = self(next);
                    area += next_area;
                    perimeter += next_perim;
                    corners += next_corners;
                }
            }
        }

        return {area, perimeter, corners};
    };

    int p1 = 0;
    int p2 = 0;

    while (!unassigned_positions.empty()) {
        auto [area, perim, corners]
            = build_region(*unassigned_positions.begin());
        p1 += area * perim;
        p2 += area * (corners / 3);
    }

    return {p1, p2};
};

[[maybe_unused]] constexpr auto& test_input1 =
    R"(AAAA
BBCD
BBCC
EEEC
)";

[[maybe_unused]] constexpr auto& test_input2 =
    R"(OOOOO
OXOXO
OOOOO
OXOXO
OOOOO
)";

[[maybe_unused]] constexpr auto& test_input3 =
    R"(RRRRIICCFF
RRRRIICCCF
VVRRRCCFFF
VVRCCCJFFF
VVVVCJJCFE
VVIVCCJJEE
VVIIICJJEE
MIIIIIJJEE
MIIISIJEEE
MMMISSJEEE
)";

[[maybe_unused]] constexpr auto& test_input4 =
    R"(EEEEE
EXXXX
EEEEE
EXXXX
EEEEE
)";

[[maybe_unused]] constexpr auto& test_input5 =
    R"(AAAAAA
AAABBA
AAABBA
ABBAAA
ABBAAA
AAAAAA
)";

} // namespace

int main(int argc, char** argv)
{
    // Part 1 tests
    {
        assert(calculate_prices(test_input1).part1 == 140);
        assert(calculate_prices(test_input2).part1 == 772);
        assert(calculate_prices(test_input3).part1 == 1930);
    }

    // Part 2 tests
    {
        assert(calculate_prices(test_input1).part2 == 80);
        assert(calculate_prices(test_input2).part2 == 436);
        assert(calculate_prices(test_input4).part2 == 236);
        assert(calculate_prices(test_input5).part2 == 368);
        assert(calculate_prices(test_input3).part2 == 1206);
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto [part1, part2] = calculate_prices(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1);
    std::println("Part 2: {}", part2);
}