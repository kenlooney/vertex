#include "osasm/parser.h"
#include "utils/integer.h"
#include <limits>

bool OsasmParser::number(std::uint64_t& value, std::string& error)
{
    if(at_end() || peek().kind != "integer") {
        error = "Expected an integer";
        return false;
    }

    const Token& token = peek();
    if(!parse_integer(token.text, value)) {
        error = "Invalid integer: " + token.text;
        return false;
    }

    advance();
    return true;
}

bool OsasmParser::sum(std::uint64_t& value, std::string& error)
{
    if(!product(value, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && peek().text == "+") {
        advance();
        std::uint64_t right;
        if(!product(right, error)) {
            return false;
        }
        if(value > std::numeric_limits<std::uint64_t>::max() - right) {
            error = "Integer overflow in expression";
            return false;
        }
        value += right;
    }
    return true;
}

bool OsasmParser::product(std::uint64_t& value, std::string& error)
{
    if(!number(value, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && peek().text == "*") {
        advance();
        std::uint64_t right;
        if(!number(right, error)) {
            return false;
        }
        if(right != 0
            && value > std::numeric_limits<std::uint64_t>::max() / right) {
            error = "Integer overflow in expression";
            return false;
        }
        value *= right;
    }
    return true;
}

bool OsasmParser::run(std::uint64_t& value, std::string& error)
{
    if(!sum(value, error)) {
        return false;
    }
    if(!at_end()) {
        error = "Unexpected token: " + peek().text;
        return false;
    }
    return true;
}

bool parse_expression(const std::vector<Token>& tokens, std::uint64_t& value, std::string& error) {
    OsasmParser parser(tokens);
    return parser.run(value, error);
}