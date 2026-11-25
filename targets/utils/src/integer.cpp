#include "utils/integer.h"

#include <cctype>
#include <limits>
#include <string>

bool parse_byte(const std::string& text, std::uint8_t& value) {
    std::uint64_t temp;
    if(!parse_integer(text, temp) || temp > std::numeric_limits<std::uint8_t>::max()) {
        return false;
    }
    value = static_cast<std::uint8_t>(temp);
    return true;
}

bool parse_word(const std::string& text, std::uint16_t& value) {
    std::uint64_t temp;
    if(!parse_integer(text, temp) || temp > std::numeric_limits<std::uint16_t>::max()) {
        return false;
    }
    value = static_cast<std::uint16_t>(temp);
    return true;
}

bool parse_integer(const std::string& text, std::uint64_t& value) {
    if(text.empty()) {
        return false;
    }

    unsigned base = 10;
    std::size_t begin = 0;
    std::size_t end = text.size();

    if(text.size() >= 2 && text[0] == '0') {
        const char prefix = text[1] >= 'A' && text[1] <= 'Z'
            ? static_cast<char>(text[1] - 'A' + 'a')
            : text[1];

        if(prefix == 'b') {
            base = 2;
            begin = 2;
        } else if(prefix == 'o') {
            base = 8;
            begin = 2;
        } else if(prefix == 'd') {
            base = 10;
            begin = 2;
        } else if(prefix == 'x') {
            base = 16;
            begin = 2;
        }
    }

    if(begin == 0 && text.size() > 1) {
        const char suffix = text.back() >= 'A' && text.back() <= 'Z'
            ? static_cast<char>(text.back() - 'A' + 'a')
            : text.back();

        if(suffix == 'b') {
            base = 2;
            --end;
        } else if(suffix == 'o' || suffix == 'q') {
            base = 8;
            --end;
        } else if(suffix == 'd') {
            base = 10;
            --end;
        } else if(suffix == 'h') {
            base = 16;
            --end;
        }
    }

    if(begin == end) {
        return false;
    }

    std::uint64_t result = 0;
    for(std::size_t pos = begin; pos < end; ++pos) {
        const char ch = text[pos];
        unsigned digit;
        if(ch >= '0' && ch <= '9') {
            digit = static_cast<unsigned>(ch - '0');
        } else if(ch >= 'a' && ch <= 'f') {
            digit = static_cast<unsigned>(ch - 'a') + 10;
        } else if(ch >= 'A' && ch <= 'F') {
            digit = static_cast<unsigned>(ch - 'A') + 10;
        } else {
            return false;
        }

        if(digit >= base) {
            return false;
        }
        if(result > (std::numeric_limits<std::uint64_t>::max() - digit) / base) {
            return false;
        }
        result = result * base + digit;
    }

    value = result;
    return true;
}