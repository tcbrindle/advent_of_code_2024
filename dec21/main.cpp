
#include <aoc.hpp>

namespace {

using i64 = std::int64_t;

using namespace std::string_view_literals;

auto const numpad_idx = [](char c) {
    if (c == 'A') {
        return 10;
    } else {
        return c - '0';
    }
};

auto const dirpad_idx = [](char c) {
    switch (c) {
    case '^': return 0;
    case 'A': return 1;
    case '<': return 2;
    case 'v': return 3;
    case '>': return 4;
    default: std::unreachable();
    };
};

constexpr std::string_view numpad_moves_table[11][11] = {
    /* From 0 */
    {"A", "^<A", "^A", ">^A", "^^<A", "^^A", ">^^A", "^^^<A", "^^^A", ">^^^A",
     ">A"},
    /* From 1*/
    {">vA", "A", ">A", ">>A", "^A", "^>A", "^>>A", "^^A", "^^>A", "^^>>A",
     ">>vA"},
    /* From 2 */
    {"<vA", "<A", "A", ">A", "<^A", "^A", "^>A", "<^^A", "^^A", "^^>A", "v>A"},
    /* From 3 */
    {"<vA", "<<A", "<A", "A", "<<^A", "<^A", "^A", "<<^^A", "<^^A", "^^A",
     "vA"},
    /* From 4 */
    {">vvA", "vA", "v>A", "v>>A", "A", ">A", ">>A", "^A", "^>A", "^>>A",
     ">>vvA"},
    /* From 5 */
    {"vvA", "<vA", "vA", "v>A", "<A", "A", ">A", "<^A", "^A", "^>A", "vv>A"},
    /* From 6 */
    {"<vvA", "<<vA", "<vA", "vA", "<<A", "<A", "A", "<<^A", "<A", "^A", "vvA"},
    /* From 7 */
    {">vvvA", "vvA", "vv>A", "vv>>A", "vA", "v>A", "v>>A", "A", ">A", ">>A",
     ">>vvvA"},
    /* From 8 */
    {"vvvA", "<vvA", "vvA", "vv>A", "<vA", "vA", "v>A", "<A", "A", ">A",
     "vvv>A"},
    /* From 9 */
    {"<vvvA", "<<vvA", "<vvA", "vvA", "<<vA", "<vA", "vA", "<<A", "<A", "A",
     "vvvA"},
    /* From A */
    {"<A", "^<<A", "<^A", "^A", "^^<<A", "<^^A", "^^A", "^^^<<A", "<^^^A",
     "^^^A", "A"},
};

constexpr std::string_view dirpad_moves_table[5][5] = {
    /* From ^ */ {"A", ">A", "v<A", "vA", "v>A"},
    /* From A */ {"<A", "A", "v<<A", "<vA", "vA"},
    /* From < */ {">^A", ">>^A", "A", ">A", ">>A"},
    /* From v */ {"^A", "^>A", "<A", "A", ">A"},
    /* From > */ {"<^A", "^A", "<<A", "<A", "A"},
};

template <int N>
using cache_t = std::array<aoc::hash_map<std::string, i64>, N>;

auto get_length_recursive(std::string str, int depth, auto& cache) -> i64
{
    if (depth == 0) {
        return str.size();
    }

    if (auto iter = cache[depth].find(str); iter != cache[depth].end()) {
        return iter->second;
    }

    i64 len
        = flux::chain(flux::single('A'), flux::ref(str))
              .pairwise_map([&](char from, char to) {
                  return dirpad_moves_table[dirpad_idx(from)][dirpad_idx(to)];
              })
              .map([&](std::string_view next) {
                  return get_length_recursive(std::string(next), depth - 1,
                                              cache);
              })
              .sum();

    cache[depth][std::move(str)] = len;

    return len;
};

template <int Levels>
auto const calculate_complexity = [](std::string_view input) {
    i64 num = flux::filter(input, ::isdigit)._(aoc::parse<i64>);

    cache_t<Levels + 1> cache{};
    for (auto& map : cache) {
        map.reserve(16);
    }

    auto len
        = flux::chain(flux::single('A'), input)
              .pairwise_map([&](char from, char to) {
                  return numpad_moves_table[numpad_idx(from)][numpad_idx(to)];
              })
              .map([&](std::string_view str) {
                  return get_length_recursive(std::string(str), Levels, cache);
              })
              .sum();

    return num * len;
};

template <int Levels>
auto const calculate_all = [](std::string_view input) {
    return flux::split_string(input, '\n')
        .filter(std::not_fn(flux::is_empty))
        .map(calculate_complexity<Levels>)
        .sum();
};

auto const part1 = calculate_all<2>;
auto const part2 = calculate_all<25>;

[[maybe_unused]] constexpr auto& test_input =
    R"(029A
980A
179A
456A
379A
)";

} // namespace

int main(int argc, char** argv)
{
    assert(part1(test_input) == 126384);

    if (argc < 2) {
        std::println(stderr, "No input");
        return 1;
    }

    auto const input = aoc::string_from_file(argv[1]);
    std::println("Part 1: {}", part1(input));
    std::println("Part 2: {}", part2(input));
}