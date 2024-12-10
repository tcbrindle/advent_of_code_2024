
#include <aoc.hpp>

namespace {

using int_t = uint16_t;

constexpr auto empty = std::numeric_limits<int_t>::max();

auto const parse_input_p1 = [](std::string_view input) {
    std::vector<int_t> out;
    for (auto [c, n] : flux::zip(input, flux::ints())) {
        for (int i = 0; i < c - '0'; i++) {
            if (n % 2 == 0) {
                out.push_back(n / 2);
            } else {
                out.push_back(empty);
            }
        }
    }
    return out;
};

auto const part1 = [](std::string_view input) {
    auto data = parse_input_p1(input);

    // Indices of empty positions, from the front
    auto forward = flux::ints(0, data.size()).filter([&](auto i) {
        return data.at(i) == empty;
    });
    // Indices of non-empty positions, from the back
    auto backward = flux::ints(0, data.size()).reverse().filter([&](auto i) {
        return data.at(i) != empty;
    });

    // Iterate in lock-step until the indices meet, swapping each pair
    flux::zip(std::move(forward), std::move(backward))
        .take_while(flux::unpack(std::less{}))
        .for_each(flux::unpack(
            [&data](auto a, auto b) { flux::swap_at(data, a, b); }));

    // Calculate the checksum
    return flux::zip_map(std::multiplies{},
                         flux::ref(data).filter(flux::pred::neq(empty)),
                         flux::ints())
        .sum();
};

struct span {
    int start;
    int size;
};

auto parse_input_p2 = [](std::string_view input)
    -> std::pair<std::vector<span>, std::vector<span>> {
    std::vector<span> files;
    std::vector<span> spaces;
    int idx = 0;
    for (auto [c, n] : flux::zip(input, flux::ints())) {
        int sz = c - '0';
        if (n % 2 == 0) {
            files.push_back({idx, sz});
        } else {
            spaces.push_back({idx, sz});
        }
        idx += sz;
    }
    return {std::move(files), std::move(spaces)};
};

auto const part2 = [](std::string_view input) {
    auto [files, spaces] = parse_input_p2(input);

    for (auto& f : flux::mut_ref(files).reverse()) {
        auto cur = flux::find_if(spaces, [&](span space) {
            return space.size >= f.size && space.start < f.start;
        });
        if (!flux::is_last(spaces, cur)) {
            spaces.push_back(auto(f));
            f.start = spaces[cur].start;
            spaces[cur].start += f.size;
            spaces[cur].size -= f.size;
        }
    }

    // Calculate checksum
    return flux::zip_map(
               [](span s, int i) {
                   return flux::ints(s.start, s.start + s.size)
                       .map([i](auto j) { return i * j; })
                       .sum();
               },
               flux::ref(files), flux::ints())
        .sum();
};

constexpr auto& test_input = "2333133121414131402";

static_assert(part1(test_input) == 1928);
static_assert(part2(test_input) == 2858);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const input = aoc::string_from_file(argv[1]);

    std::println("Part 1: {}", part1(input));
    std::println("Part 2: {}", aoc::timed(part2, input));
}