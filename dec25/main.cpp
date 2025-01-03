
#include <aoc.hpp>

namespace {

using pins_t = std::array<int8_t, 5>;

auto const parse_input = [](std::string_view input)
    -> std::pair<std::vector<pins_t>, std::vector<pins_t>> {
    std::vector<pins_t> keys;
    std::vector<pins_t> locks;
    auto read_pins = [](pins_t pins, auto chunk) {
        for (auto [c, i] : flux::zip(std::move(chunk), flux::ints())) {
            if (c == '#') {
                ++pins.at(i);
            }
        }
        return pins;
    };

    flux::split_string(input, "\n\n").for_each([&](std::string_view block) {
        if (block.at(0) == '#') {
            // Reading a key
            auto pins = flux::filter(block, flux::pred::neq('\n'))
                            .chunk(5)
                            .drop(1)
                            .fold(read_pins, pins_t{});

            keys.push_back(pins);
        } else {
            // Reading a lock
            std::string str
                = flux::filter(block, flux::pred::neq('\n')).to<std::string>();
            auto pins = flux::chunk(std::move(str), 5)
                            .reverse()
                            .drop(1)
                            .fold(read_pins, pins_t{});

            locks.push_back(pins);
        }
    });

    return {std::move(keys), std::move(locks)};
};

auto part1 = [](std::vector<pins_t> const& keys,
                std::vector<pins_t> const& locks) -> int {
    return flux::cartesian_product(flux::ref(keys), flux::ref(locks))
        .count_if(flux::unpack([](pins_t const& key, pins_t const& lock) {
            return flux::ints(0, 5).all(
                [&](int i) { return key.at(i) + lock.at(i) <= 5; });
        }));
};

constexpr auto& test_input = R"(#####
.####
.####
.####
.#.#.
.#...
.....

#####
##.##
.#.##
...##
...#.
...#.
.....

.....
#....
#....
#...#
#.#.#
#.###
#####

.....
.....
#.#..
###..
###.#
###.#
#####

.....
.....
.....
#....
#.#..
#.#.#
#####
)";

static_assert([] {
    auto [keys, locks] = parse_input(test_input);
    return part1(keys, locks) == 3;
}());

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const [keys, locks] = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1(keys, locks));
}