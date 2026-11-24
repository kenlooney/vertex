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

bool OsasmParser::primary(std::uint64_t& value, std::string& error)
{
    if(!at_end() && peek().kind == "punctuation" && peek().text == "(") {
        advance();

        std::uint64_t grouped;
        if(!sum(grouped, error)) {
            return false;
        }
        if(at_end() || peek().kind != "punctuation" || peek().text != ")") {
            error = "Expected ')'";
            return false;
        }

        advance();
        value = grouped;
        return true;
    }

    return number(value, error);
}

bool OsasmParser::sum(std::uint64_t& value, std::string& error)
{
    if(!product(value, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && (peek().text == "+" || peek().text == "-")) {
        const std::string op = peek().text;
        advance();
        std::uint64_t right;
        if(!product(right, error)) {
            return false;
        }
              
        if(op == "+") {
            if(value > std::numeric_limits<std::uint64_t>::max() - right) {
                error = "Integer overflow in expression";
                return false;
            }
            value += right;
        } 
        else if(op == "-") {
            if(value < right) {
                error = "Integer underflow in expression";
                return false;
            }
            value -= right;
        }
    }
    return true;
}

bool OsasmParser::product(std::uint64_t& value, std::string& error)
{
    if(!primary(value, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && (peek().text == "*" || peek().text == "/" || peek().text == "%")) {
        const std::string op = peek().text;
        advance();
        std::uint64_t right;
        if(!primary(right, error)) {
            return false;
        }
        if(op == "*") {
            if(right != 0
                && value > std::numeric_limits<std::uint64_t>::max() / right) {
                error = "Integer overflow in expression";
                return false;
            }
            value *= right;
        } 
        else if(op == "/") {
            if(right == 0) {
                error = "Division by zero in expression";
                return false;
            }
            value /= right;
        }
        else if(op == "%") {
            if(right == 0) {
                error = "Modulo by zero in expression";
                return false;
            }
            value %= right;
        }
    }
    return true;
}

bool OsasmParser::run(std::uint64_t& value, std::string& error)
{
    std::uint64_t parsed_value;
    if(!sum(parsed_value, error)) {
        return false;
    }
    if(!at_end()) {
        error = "Unexpected token: " + peek().text;
        return false;
    }

    value = parsed_value;
    return true;
}

bool parse_expression(const std::vector<Token>& tokens, std::uint64_t& value, std::string& error) {
    OsasmParser parser(tokens);
    return parser.run(value, error);
}