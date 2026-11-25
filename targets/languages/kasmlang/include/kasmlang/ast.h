#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

struct Node {
    static constexpr std::size_t no_child =
        std::numeric_limits<std::size_t>::max();

    std::string kind;
    std::uint64_t value = 0;
    std::size_t left = no_child;
    std::size_t right = no_child;
};

struct Expression {
    std::vector<Node> nodes;
    std::size_t root = Node::no_child;
};