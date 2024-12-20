
#include <aoc.hpp>

namespace {

using position = aoc::vec2_t<int>;
using direction = aoc::vec2_t<int>;

namespace dir {
constexpr direction north{0, -1};
constexpr direction east{1, 0};
constexpr direction south{0, 1};
constexpr direction west{-1, 0};
} // namespace dir

struct grid2d {
    std::string data;
    int width;
    int height;

    constexpr auto is_in_bounds(position const& pos) const -> bool
    {
        return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
    }

    constexpr auto positions() const -> flux::random_access_sequence auto
    {
        return flux::cartesian_product_map(
            [](int j, int i) { return position{i, j}; }, flux::ints(0, height),
            flux::ints(0, width));
    }

    constexpr auto to_string() const -> std::string
    {
        return flux::ref(data)
            .chunk(width)
            .flatten_with('\n')
            .to<std::string>();
    }

    constexpr auto operator[](this auto& self, position const& pos) -> auto&
    {
        return self.data.at(pos.y * self.width + pos.x);
    }
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{.data = flux::filter(input, [](char c) { return c != '\n'; })
                              .to<std::string>(),
                  .width = int(flux::find(input, '\n')),
                  .height = int(flux::count_eq(input, '\n'))};
};

auto const manhattan_dist = [](position const& p1, position const& p2) {
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
};

auto const walk_path = [](grid2d grid) -> std::vector<position> {
    std::vector<position> path;
    int start_idx = flux::find(grid.data, 'S');
    auto start_pos = position{start_idx % grid.width, start_idx / grid.width};

    [&](this auto const& self, position pos) -> void {
        path.push_back(pos);
        grid[pos] = '#';

        for (direction d : {dir::north, dir::east, dir::south, dir::west}) {
            auto next_pos = pos + d;
            if (grid.is_in_bounds(next_pos) && grid[next_pos] != '#') {
                self(next_pos);
            }
        }
    }(start_pos);
    return path;
};

template <int Dist>
constexpr auto calculate = [](grid2d const& grid) {
    auto const path = walk_path(grid);

    return flux::ints(0, path.size())
        .map([&](int i) {
            return flux::ints(i + 1, path.size())
                .map([&](int j) {
                    auto dist = manhattan_dist(path[i], path[j]);
                    return dist <= Dist ? j - i - dist : 0;
                })
                .count_if(flux::pred::geq(100));
        })
        .sum();
};

constexpr auto part1 = calculate<2>;
constexpr auto part2 = calculate<20>;

[[maybe_unused]] constexpr auto& test_input =
    R"(###############
#...#...#.....#
#.#.#.#.#.###.#
#S#...#.#.#...#
#######.#.#.###
#######.#.#...#
#######.#.###.#
###..E#...#...#
###.#######.###
#...###...#...#
#.#####.#.###.#
#.#...#.#.#...#
#.#.#.#.#.#.###
#...#...#...###
###############
)";

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(grid));
    std::println("Part 2: {}", part2(grid));
}