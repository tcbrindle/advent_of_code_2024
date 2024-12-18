
#include <aoc.hpp>
#include <queue>
#include <ctre.hpp>

namespace {

using position = aoc::vec2_t<int>;

auto const parse_input = [](std::string_view input) -> std::vector<position> {
    return flux::from(ctre::search_all<"(\\d+),(\\d+)">(input))
        .map([](auto result) {
            auto [_, a, b] = result;
            return position{a.to_number(), b.to_number()};
        })
        .to<std::vector>();
};

template <typename G>
concept Graph
    = std::regular<typename G::node_type> && std::regular<typename G::cost_type>
    && requires(G const& graph, G::node_type const& n) {
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

    queue_t queue;
    cost_map_t costs;

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
            }
        });
    }

    return costs;
};

template <int Size>
struct grid_t {
    std::bitset<Size * Size> data;

    static constexpr position target{Size - 1, Size - 1};

    constexpr auto operator[](this auto& self, position const& pos)
    {
        return self.data[pos.y * Size + pos.x];
    }
};

template <int Size>
struct graph_t {
    grid_t<Size> const& grid;

    using node_type = position;
    using cost_type = int;

    constexpr auto get_neighbours(position const& pos) const
    {
        return flux::from(std::array<position, 4>{
                              position{0, -1}, {1, 0}, {0, 1}, {-1, 0}})
            .map([&](position o) { return pos + o; })
            .filter([&](position n) {
                return n.x >= 0 && n.x < Size && n.y >= 0 && n.y < Size
                    && !grid[n];
            });
    }

    constexpr auto get_cost(position const&, position const&) const -> int
    {
        return 1;
    }

    constexpr auto should_exit(position const& p) const -> bool
    {
        return p == grid.target;
    }
};

template <int GridSize, std::size_t N>
auto const part1 = [](std::span<position const> bytes) -> int {
    grid_t<GridSize> grid{};

    flux::for_each(bytes.first<N>(),
                   [&](position const& pos) { grid[pos] = true; });

    return dijkstra(graph_t{grid}, {0, 0}).at(grid.target);
};

template <int GridSize, std::size_t Skip>
auto const part2 = [](std::span<position const> bytes) -> position {
    auto idx = *std::ranges::partition_point(
        std::views::iota(Skip, bytes.size()), [&](std::size_t n) {
            grid_t<GridSize> grid{};
            flux::for_each(bytes.first(n + 1),
                           [&](position const& pos) { grid[pos] = true; });
            return dijkstra(graph_t{grid}, {0, 0}).contains(grid.target);
        });
    return bytes[idx];
};

constexpr auto& test_input = R"(5,4
4,2
4,5
3,0
2,1
6,3
2,4
1,5
0,6
3,3
2,6
5,1
1,2
5,5
2,5
6,5
1,4
0,4
6,4
1,1
6,1
1,0
0,5
1,6
2,0
)";

} // namespace

int main(int argc, char** argv)
{
    {
        [[maybe_unused]] auto const test_bytes = parse_input(test_input);
        assert((part1<7, 12>(test_bytes) == 22));
        assert((part2<7, 12>(test_bytes) == position{6, 1}));
    }

    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const bytes = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1<71, 1024>(bytes));
    std::println("Part 2: {}", part2<71, 1024>(bytes));
}