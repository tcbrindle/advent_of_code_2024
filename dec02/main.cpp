
#include <aoc.hpp>

namespace {

auto parse_input = [](std::string_view input) -> std::vector<std::vector<int>> {
    return flux::split_string(input, '\n')
        .filter([](std::string_view line) { return !line.empty(); })
        .map([](std::string_view line) {
            return flux::split_string(line, ' ')
                .map(aoc::parse<int>)
                .to<std::vector<int>>();
        })
        .to<std::vector>();
};

auto abs_diff = []<flux::num::integral T>(T const& lhs, T const& rhs) -> T {
    T diff = flux::num::sub(lhs, rhs);
    return diff < T{0} ? flux::num::neg(diff) : diff;
};

auto is_sorted
    = [](flux::multipass_sequence auto const& seq, auto cmp) -> bool {
    return flux::ref(seq).pairwise_map(cmp).all(std::identity{});
};

auto is_safe = [](flux::multipass_sequence auto const& seq) -> bool {
    return (is_sorted(seq, std::less{}) || is_sorted(seq, std::greater{}))
        && flux::ref(seq).pairwise_map(abs_diff).all(flux::pred::geq(1)
                                                     && flux::pred::leq(3));
};

// Returns an adapted sequence which skips the element at index n
auto skip_n = [](flux::sequence auto&& seq, flux::index_t n) {
    return flux::zip(FLUX_FWD(seq), flux::ints())
        .filter([n](auto pair) { return pair.second != n; })
        .map([](auto pair) -> decltype(auto) { return pair.first; });
};

auto part1 = [](std::vector<std::vector<int>> const& vec) -> int {
    return flux::count_if(vec, is_safe);
};

auto part2 = [](std::vector<std::vector<int>> const& vec) -> int {
    return flux::count_if(vec, [](auto const& report) -> bool {
        return is_safe(report)
            || flux::ints(0, report.size())
                   .map([r = flux::ref(report)](int i) { return skip_n(r, i); })
                   .any(is_safe);
    });
};

constexpr auto& test_data =
    R"(7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
)";

static_assert([] {
    auto vec = parse_input(test_data);
    return part1(vec) == 2 && part2(vec) == 4;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        fmt::println(stderr, "No input");
        return -1;
    }

    auto const input = parse_input(aoc::string_from_file(argv[1]));

    fmt::println("Part 1: {}", part1(input));
    fmt::println("Part 2: {}", part2(input));
}