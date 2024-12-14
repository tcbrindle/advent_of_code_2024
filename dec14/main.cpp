
#include <aoc.hpp>
#include <ctre.hpp>

namespace {

constexpr bool enable_part2 = false;

using vec2 = aoc::vec2_t<int>;

struct robot {
    vec2 pos;
    vec2 vel;
};

auto const parse_input = [](std::string_view input) -> std::vector<robot> {
    constexpr auto& regex = R"(p=(-?\d+),(-?\d+) v=(-?\d+),(-?\d+)\n?)";
    return flux::map(ctre::tokenize<regex>(input),
                     [](auto result) {
                         auto [_, px, py, vx, vy] = result;
                         return robot{.pos = {px.to_number(), py.to_number()},
                                      .vel = {vx.to_number(), vy.to_number()}};
                     })
        .to<std::vector>();
};

template <vec2 Bounds>
auto const part1 = [](std::vector<robot> robots) -> int64_t {
    // Move robots
    for (auto& [pos, vel] : robots) {
        pos += 100 * (vel + Bounds);
        pos.x %= Bounds.x;
        pos.y %= Bounds.y;
    }

    // Calculate safety factor
    std::array<int64_t, 4> quadrants{};
    for (auto const& [pos, _] : robots) {
        if (pos.x < Bounds.x / 2) {
            if (pos.y < Bounds.y / 2) {
                ++quadrants[0];
            } else if (pos.y > Bounds.y / 2) {
                ++quadrants[1];
            }
        } else if (pos.x > Bounds.x / 2) {
            if (pos.y < Bounds.y / 2) {
                ++quadrants[2];
            } else if (pos.y > Bounds.y / 2) {
                ++quadrants[3];
            }
        }
    }
    return flux::product(quadrants);
};

template <vec2 Bounds>
auto const print_robots =
    [](int secs, std::vector<robot> const& robots, std::ofstream& where) {
        std::vector<std::string> strings(Bounds.y, std::string(Bounds.x, '.'));

        for (auto const& [pos, _] : robots) {
            strings.at(pos.y).at(pos.x) = '*';
        }

        std::println(where, "\n\n\nAfter {} seconds:", secs);
        for (auto const& s : strings) {
            std::println(where, "{}", s);
        }
    };

template <vec2 Bounds>
auto const part2 = [](std::vector<robot> robots, std::ofstream& os) {
    for (auto counter : flux::ints(0, 10000)) {
        print_robots<Bounds>(counter, robots, os);

        for (auto& [pos, vel] : robots) {
            pos += vel + Bounds;
            pos.x %= Bounds.x;
            pos.y %= Bounds.y;
        }
    }
};

constexpr auto& test_input =
    R"(p=0,4 v=3,-3
p=6,3 v=-1,-3
p=10,3 v=-1,2
p=2,0 v=2,-1
p=0,0 v=1,3
p=3,0 v=-2,-2
p=7,6 v=-1,-3
p=3,0 v=-1,-2
p=9,3 v=2,3
p=7,3 v=-1,2
p=2,4 v=2,-3
p=9,5 v=-3,-3)";

static_assert(part1<{11, 7}>(parse_input(test_input)) == 12);

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::println(stderr, "No input");
        return -1;
    }

    auto const robots = parse_input(aoc::string_from_file(argv[1]));
    std::println("Part 1: {}", part1<{101, 103}>(robots));

    if constexpr (enable_part2) {
        std::ofstream out("output.txt");
        part2<{101, 103}>(robots, out);
    }
}