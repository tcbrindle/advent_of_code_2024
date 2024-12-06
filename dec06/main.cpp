
#include <aoc.hpp>

namespace {

using i64 = std::int64_t;

enum class direction { north, east, south, west };

constexpr auto operator++(direction& d) -> direction&
{
    d = direction{(int(d) + 1) % 4};
    return d;
}

struct position {
    i64 x = 0;
    i64 y = 0;

    friend constexpr auto operator==(position const&, position const&) -> bool
        = default;

    friend constexpr auto operator+(position const& p, direction d) -> position
    {
        using enum direction;
        switch (d) {
        case north: return {p.x, p.y - 1};
        case east: return {p.x + 1, p.y};
        case south: return {p.x, p.y + 1};
        case west: return {p.x - 1, p.y};
        }
    }
};

struct grid2d {
    std::string data;
    i64 width;
    i64 height;

    using index_type = i64;

    constexpr auto is_in_bounds(position p) const -> bool
    {
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }

    constexpr auto operator[](this auto& self, position p) -> auto&
    {
        return self.data.at(self.to_index(p));
    }

    constexpr auto to_index(position p) const -> index_type
    {
        return p.y * width + p.x;
    }

    constexpr auto to_position(index_type idx) const -> position
    {
        return {idx % height, idx / width};
    }
};

auto parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .width = flux::find(input, '\n'),
        .height = flux::count_eq(input, '\n')};
};

using guard_state = std::array<bool, 4>;

auto walk_grid = [](grid2d const& grid,
                    position pos) -> std::optional<std::vector<guard_state>> {
    std::vector<guard_state> states(grid.data.size());
    direction dir = direction::north;

    while (true) {
        if (bool& visited = states.at(grid.to_index(pos)).at(int(dir));
            visited) {
            return std::nullopt;
        } else {
            visited = true;
        }

        auto next_pos = pos + dir;
        if (!grid.is_in_bounds(next_pos)) {
            break;
        }
        while (grid[next_pos] == '#') {
            ++dir;
            next_pos = pos + dir;
        }
        pos = next_pos;
    }

    return states;
};

auto find_start = [](grid2d const& grid) -> position {
    auto idx = flux::find(grid.data, '^');
    return grid.to_position(idx);
};

auto part1 = [](grid2d const& grid) -> i64 {
    return flux::from(walk_grid(grid, find_start(grid)).value())
        .count_if(std::bind_back(flux::any, flux::pred::id));
};

auto part2 = [](grid2d const& grid) -> i64 {
    auto const start_pos = find_start(grid);

    return flux::zip(walk_grid(grid, start_pos).value(), flux::ints())
        .filter([](auto pair) { return flux::any(pair.first, flux::pred::id); })
        .map([&grid](auto pair) { return grid.to_position(pair.second); })
        .count_if([&](position pos) {
            if (pos == start_pos) {
                return false;
            }

            grid2d copy = grid;
            copy[pos] = '#';

            return !walk_grid(copy, start_pos).has_value();
        });
};

constexpr auto& test_input =
    R"(....#.....
.........#
..........
..#.......
.......#..
..........
.#..^.....
........#.
#.........
......#...
)";

static_assert([] {
    auto grid = parse_input(test_input);
    return part1(grid) == 41 && part2(grid) == 6;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    grid2d const grid = parse_input(aoc::string_from_file(argv[1]));

    std::println("Part 1 test: {}", aoc::timed(part1, grid));
    std::println("Part 2 test: {}",
                 aoc::timed<std::chrono::milliseconds>(part2, grid));
}