#include "osasm/parser.h"

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

Token token(std::string kind, std::string text) {
    static std::size_t begin = 0;
    const std::size_t end = begin + text.size();
    Token result{std::move(kind), std::move(text), Span::set(begin, end)};
    begin = end;
    return result;
}

int main() {
    std::uint64_t value = 0;
    std::string error;
    const std::vector<Token> expression = {
        token("integer", "0xA"),
        token("punctuation", "+"),
        token("integer", "0101b"),
    };

    if(!parse_expression(expression, value, error) || value != 15) {
        std::cerr << "Failed to parse a valid expression: " << error << '\n';
        return 1;
    }
    std::cout << "Parsed expression value: " << value << '\n';

    const std::vector<Token> precedence = {
        token("integer", "2"),
        token("punctuation", "+"),
        token("integer", "3"),
        token("punctuation", "*"),
        token("integer", "4"),
    };
    if(!parse_expression(precedence, value, error) || value != 14) {
        std::cerr << "Failed to honor operator precedence: " << error << '\n';
        return 1;
    }

    const std::vector<Token> zero_product = {
        token("integer", "0"),
        token("punctuation", "*"),
        token("integer", "18446744073709551615"),
    };
    if(!parse_expression(zero_product, value, error) || value != 0) {
        std::cerr << "Failed to parse multiplication by zero: " << error << '\n';
        return 1;
    }

    const std::vector<Token> grouped = {
        token("punctuation", "("),
        token("integer", "2"),
        token("punctuation", "+"),
        token("integer", "3"),
        token("punctuation", ")"),
        token("punctuation", "*"),
        token("integer", "4"),
    };
    if(!parse_expression(grouped, value, error) || value != 20) {
        std::cerr << "Failed to parse grouped expression: " << error << '\n';
        return 1;
    }

    const std::vector<Token> nested_group = {
        token("punctuation", "("),
        token("integer", "2"),
        token("punctuation", "*"),
        token("punctuation", "("),
        token("integer", "3"),
        token("punctuation", "+"),
        token("integer", "4"),
        token("punctuation", ")"),
        token("punctuation", ")"),
    };
    if(!parse_expression(nested_group, value, error) || value != 14) {
        std::cerr << "Failed to parse nested grouped expression: " << error << '\n';
        return 1;
    }

    const std::vector<Token> unclosed_group = {
        token("punctuation", "("),
        token("integer", "2"),
        token("punctuation", "+"),
        token("integer", "3"),
    };
    if(parse_expression(unclosed_group, value, error) || error != "Expected ')'") {
        std::cerr << "Failed to reject an unclosed group\n";
        return 1;
    }

    const std::vector<Token> arithmetic = {
        token("punctuation", "("),
        token("integer", "3"),
        token("punctuation", "+"),
        token("punctuation", "("),
        token("integer", "2"),
        token("punctuation", "*"),
        token("punctuation", "("),
        token("integer", "20"),
        token("punctuation", "-"),
        token("integer", "5"),
        token("punctuation", ")"),
        token("punctuation", "+"),
        token("integer", "4"),
        token("punctuation", ")"),
        token("punctuation", ")"),
    };
    if(!parse_expression(arithmetic, value, error) || value != 37) {
        std::cerr << "Failed to parse arithmetic expression: " << error << '\n';
        return 1;
    }

    const std::vector<Token> division = {
        token("integer", "20"),
        token("punctuation", "/"),
        token("integer", "5"),
        token("punctuation", "%"),
        token("integer", "3"),
    };
    if(!parse_expression(division, value, error) || value != 1) {
        std::cerr << "Failed to parse division/modulo expression: " << error << '\n';
        return 1;
    }

    const std::vector<Token> division_by_zero = {
        token("integer", "10"),
        token("punctuation", "/"),
        token("integer", "0"),
    };
    if(parse_expression(division_by_zero, value, error)
        || error != "Division by zero in expression") {
        std::cerr << "Failed to reject division by zero\n";
        return 1;
    }

    const std::vector<Token> overflow = {
        token("integer", "18446744073709551615"),
        token("punctuation", "+"),
        token("integer", "1"),
    };
    std::uint64_t overflow_value = 0;
    if(parse_expression(overflow, overflow_value, error)) {
        std::cerr << "Unexpectedly accepted overflow\n";
        return 1;
    }

    return 0;
}
