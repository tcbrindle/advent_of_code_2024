
#include <aoc.hpp>
#include <queue>

namespace {

using position = aoc::vec2_t<int>;
using direction = aoc::vec2_t<int>;

namespace dir {
constexpr direction north{0, -1};
constexpr direction east{1, 0};
constexpr direction south{0, 1};
constexpr direction west{-1, 0};
} // namespace dir

struct grid2d : flux::inline_iter_base<grid2d> {
    std::string data;
    int width;
    int height;

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

    struct flux_iter_traits : flux::default_iter_traits {
        static constexpr auto first(grid2d const&) { return position{0, 0}; }

        static constexpr auto last(grid2d const& self)
        {
            return position{0, self.height};
        }

        static constexpr auto is_last(grid2d const& self, position const& pos)
        {
            return pos == last(self);
        }

        static constexpr auto inc(grid2d const& self, position& pos)
        {
            if (++pos.x == self.width) {
                pos.x = 0;
                ++pos.y;
            }
        }

        static constexpr auto read_at(auto& self, position const& pos) -> auto&
        {
            return self.data.at(pos.y * self.width + pos.x);
        }

        static constexpr auto size(auto const& self)
        {
            return flux::num::mul(self.width, self.height);
        }
    };
};

auto const parse_input = [](std::string_view input) -> grid2d {
    return grid2d{
        .data = flux::filter(input, flux::pred::neq('\n')).to<std::string>(),
        .width = int(flux::find(input, '\n')),
        .height = int(flux::count_eq(input, '\n'))};
};

template <typename G>
concept Graph
    = std::regular<typename G::node_type> && std::regular<typename G::cost_type>
    && requires(G& graph, G::node_type const& n) {
           { graph.get_neighbours(n) } -> flux::iterable; // of G::node_type
           { graph.get_cost(n, n) } -> std::same_as<typename G::cost_type>;
           { graph.should_exit(n) } -> std::same_as<bool>;
       };

auto const dijkstra = []<Graph G>(G const& graph, G::node_type start) {
    using node_t = G::node_type;
    using cost_t = G::cost_type;
    using queue_t = std::priority_queue<std::pair<cost_t, node_t>,
                                        std::vector<std::pair<cost_t, node_t>>,
                                        std::greater<>>;
    using cost_map_t = aoc::hash_map<node_t, cost_t>;
    using paths_map_t = aoc::hash_map<node_t, std::vector<node_t>>;

    queue_t queue;
    cost_map_t costs;
    paths_map_t paths;

    queue.push({0, start});

    while (!queue.empty()) {
        auto [current_cost, current] = queue.top();
        queue.pop();

        if (graph.should_exit(current)) {
            break;
        }

        flux::for_each(graph.get_neighbours(current), [&](node_t const& next) {
            cost_t next_cost = current_cost + graph.get_cost(current, next);

            if (auto iter = costs.find(next);
                iter == costs.cend() || next_cost < iter->second) {
                costs[next] = next_cost;
                queue.push({next_cost, next});
                paths[next].push_back(current);
            } else if (next_cost == iter->second) {
                paths[next].push_back(current);
            }
        });
    }

    return std::pair(std::move(costs), std::move(paths));
};

struct graph {
    grid2d const& grid;

    using node_type = std::pair<position, direction>;
    using cost_type = int;

    constexpr auto get_neighbours(node_type const& node) const
    {
        return flux::from(
                   std::array{dir::north, dir::east, dir::south, dir::west})
            .filter([&](direction d) { return d != -1 * node.second; })
            .map([&](direction d) { return node_type(node.first + d, d); })
            .filter([&](node_type n) { return grid[n.first] != '#'; });
    }

    constexpr auto get_cost(node_type const& from, node_type const& to) const
        -> cost_type
    {
        return from.second == to.second ? 1 : 1001;
    }

    constexpr bool should_exit(node_type const& n) const
    {
        return grid[n.first] == 'E';
    }
};

auto const part1 = [](grid2d const& grid) -> int {
    return flux::filter(
               dijkstra(graph{grid}, {flux::find(grid, 'S'), dir::east}).first,
               [&](auto const& pair) { return grid[pair.first.first] == 'E'; })
        .front()
        ->second;
};

auto const part2 = [](grid2d grid) -> int {
    auto const start_pos = grid.find('S');

    auto const paths = dijkstra(graph{grid}, {start_pos, dir::east}).second;

    auto const end_node = flux::ref(paths)
                              .filter([&](auto const& pair) {
                                  return grid[pair.first.first] == 'E';
                              })
                              .front()
                              ->first;

    [&](this const auto& self, auto const& node) -> void {
        grid[node.first] = 'O';
        if (node.first != start_pos) {
            flux::for_each(paths.at(node), self);
        }
    }(end_node);

    return grid.count_eq('O');
};

constexpr auto& test_input1 =
    R"(###############
#.......#....E#
#.#.###.#.###.#
#.....#.#...#.#
#.###.#####.#.#
#.#.#.......#.#
#.#.#####.###.#
#...........#.#
###.#.#####.#.#
#...#.....#.#.#
#.#.#.###.#.#.#
#.....#...#.#.#
#.###.#.#.#.#.#
#S..#.....#...#
###############)";

constexpr auto& test_input2 =
    R"(#################
#...#...#...#..E#
#.#.#.#.#.#.#.#.#
#.#.#.#...#...#.#
#.#.#.#.###.#.#.#
#...#.#.#.....#.#
#.#.#.#.#.#####.#
#.#...#.#.#.....#
#.#.#####.#.###.#
#.#.#.......#...#
#.#.###.#####.###
#.#.#...#.....#.#
#.#.#.#####.###.#
#.#.#.........#.#
#.#.#.#########.#
#S#.............#
#################
)";

} // namespace

int main(int argc, char** argv)
{
    {
        [[maybe_unused]] auto const test_grid1 = parse_input(test_input1);
        assert(part1(test_grid1) == 7036);
        assert(part2(test_grid1) == 45);

        [[maybe_unused]] auto const test_grid2 = parse_input(test_input2);
        assert(part1(test_grid2) == 11048);
        assert(part2(test_grid2) == 64);
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const grid = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(grid));
    std::println("Part 2: {}", part2(grid));
}