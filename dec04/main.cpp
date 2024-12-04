
#include <aoc.hpp>

namespace {

using namespace std::string_view_literals;

using i64 = std::int64_t;

struct grid2d {
    std::string data;
    i64 width;
    i64 height;

    // Returns '.' if (x, y) is out of bounds
    constexpr auto operator[](i64 x, i64 y) const -> char
    {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return '.';
        }
        return data.at(y * width + x);
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .width = flux::find(input, '\n'),
        .height = flux::count_eq(input, '\n')};
};

auto const neighbours = [](grid2d const& grid, i64 x, i64 y) {
    return flux::cartesian_power<2>(flux::ints(-1, 2))
        .filter(flux::unpack([](i64 i, i64 j) { return !(i == 0 && j == 0); }))
        .map(flux::unpack([&grid, x, y](i64 i, i64 j) -> std::string {
            return {grid[x + i, y + j], grid[x + 2 * i, y + 2 * j],
                    grid[x + 3 * i, y + 3 * j]};
        }));
};

auto const part1 = [](grid2d const& grid) -> i64 {
    return flux::cartesian_product(flux::ints(0, grid.height),
                                   flux::ints(0, grid.width))
        .filter(flux::unpack([&](i64 x, i64 y) { return grid[x, y] == 'X'; }))
        .map(flux::unpack([&](i64 x, i64 y) { return neighbours(grid, x, y); }))
        .flatten()
        .count_if(flux::pred::eq("MAS"sv));
};

auto const part2 = [](grid2d const& grid) -> i64 {
    return flux::cartesian_product(flux::ints(0, grid.height - 2),
                                   flux::ints(0, grid.width - 2))
        .count_if(flux::unpack([&grid](i64 x, i64 y) {
            std::string lead{grid[x, y], grid[x + 1, y + 1],
                             grid[x + 2, y + 2]};
            std::string back{grid[x + 2, y], grid[x + 1, y + 1],
                             grid[x, y + 2]};

            return (lead == "MAS" || lead == "SAM")
                && (back == "MAS" || back == "SAM");
        }));
};

constexpr auto& test_data =
    R"(MMMSXXMASM
MSAMXMSMSA
AMXSXMAAMM
MSAMASMSMX
XMASAMXAMM
XXAMMXXAMA
SMSMSASXSS
SAXAMASAAA
MAMMMXMMMM
MXMXAXMASX
)";

static_assert(part1(parse_input(test_data)) == 18);
static_assert(part2(parse_input(test_data)) == 9);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    grid2d const grid = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(grid));
    std::println("Part 2: {}", part2(grid));
}