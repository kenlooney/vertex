#include "utils/integer.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct IntegerCase {
    std::string text;
    std::uint64_t value;
};

int main() {
    const std::vector<IntegerCase> valid_cases = {
        {"0b0101", 5},
        {"0101B", 5},
        {"0xA5", 165},
        {"0A5h", 165},
        {"A5H", 165},
        {"0o17", 15},
        {"17o", 15},
        {"17Q", 15},
        {"0d42", 42},
        {"42D", 42},
        {"42", 42},
    };

    for(const auto& test : valid_cases) {
        std::uint64_t value = 0;
        if(!parse_integer(test.text, value) || value != test.value) {
            std::cerr << "Failed to parse " << test.text << '\n';
            return 1;
        }
    }

    const std::vector<std::string> invalid_cases = {
        "",
        "0b",
        "0b102",
        "128b",
        "0o8",
        "80q",
        "0d4a",
        "0xG",
        "18446744073709551616",
    };

    for(const auto& text : invalid_cases) {
        std::uint64_t value = 0;
        if(parse_integer(text, value)) {
            std::cerr << "Unexpectedly parsed " << text << '\n';
            return 1;
        }
    }

    return 0;
}
