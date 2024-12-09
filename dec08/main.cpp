

#include <aoc.hpp>

namespace {

using position = std::pair<int, int>;

// We know we have at most (A-Z) + (a-z) + (0-9) = 62 frequencies
// So just use a lookup table rather than a hashmap
auto const select_bucket = [](char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && 'c' <= 'z') {
        return 26 + c - 'a';
    } else /*if (c >= '0' && 'c' <= '9')*/ {
        return 52 + c - '0';
    }
};

struct grid_t {
    std::array<std::vector<position>, 62> antennas;
    int width;
    int height;

    constexpr auto is_in_bounds(position const& pos) const -> bool
    {
        auto [x, y] = pos;
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

auto const parse_input = [](std::string_view input) -> grid_t {
    int x = 0;
    int y = 0;
    std::array<std::vector<position>, 62> antennas;

    for (char c : input) {
        if (c == '\n') {
            x = 0;
            ++y;
        } else if (c == '.') {
            ++x;
        } else {
            antennas.at(select_bucket(c)).push_back({x, y});
            ++x;
        }
    }

    return grid_t{.antennas = std::move(antennas),
                  .width = int(flux::find(input, '\n')),
                  .height = y};
};

template <bool Part2>
auto const calculate = [](grid_t const& grid) -> int {
    std::vector<bool> antinodes(grid.width * grid.height);
    auto add_antinode = [&](position const& p) {
        antinodes[p.second * grid.width + p.first] = true;
    };

    for (auto const& antennas : grid.antennas) {
        for (auto i : flux::ints(0, antennas.size())) {
            for (auto j : flux::ints(i + 1, antennas.size())) {
                position const p1 = antennas.at(i);
                position const p2 = antennas.at(j);

                int const x_diff = p1.first - p2.first;
                int const y_diff = p1.second - p2.second;

                position n1 = {p1.first + x_diff, p1.second + y_diff};
                position n2 = {p2.first - x_diff, p2.second - y_diff};

                if constexpr (!Part2) {
                    if (grid.is_in_bounds(n1)) {
                        add_antinode(n1);
                    }
                    if (grid.is_in_bounds(n2)) {
                        add_antinode(n2);
                    }
                } else {
                    add_antinode(p1);
                    add_antinode(p2);
                    while (grid.is_in_bounds(n1)) {
                        add_antinode(n1);
                        n1.first += x_diff;
                        n1.second += y_diff;
                    }
                    while (grid.is_in_bounds(n2)) {
                        add_antinode(n2);
                        n2.first -= x_diff;
                        n2.second -= y_diff;
                    }
                }
            }
        }
    }

    return flux::count_if(antinodes, flux::pred::eq(true));
};

auto const part1 = calculate<false>;
auto const part2 = calculate<true>;

constexpr auto& test_input =
    R"(............
........0...
.....0......
.......0....
....0.......
......A.....
............
............
........A...
.........A..
............
............
)";

static_assert([] {
    auto const grid = parse_input(test_input);
    return part1(grid) == 14 && part2(grid) == 34;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1: {}", part1(grid));
    std::println("Part 2: {}", aoc::timed(part2, grid));
}