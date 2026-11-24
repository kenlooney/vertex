#pragma once

#include "vertex/common/parser.h"
#include "vertex/common/token.h"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

bool parse_expression(const std::vector<Token>& tokens, std::uint64_t& value, std::string& error);

class OsasmParser : public Parser {
public:
    explicit OsasmParser(std::vector<Token> tokens)
        : Parser(std::move(tokens)) {}
    virtual ~OsasmParser() = default;
   
    bool number(std::uint64_t& value, std::string& error);
    bool sum(std::uint64_t& value, std::string& error);
    bool product(std::uint64_t& value, std::string& error);
    bool run(std::uint64_t& value, std::string& error);
};