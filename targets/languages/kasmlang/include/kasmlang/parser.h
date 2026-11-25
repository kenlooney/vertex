#pragma once

#include "vertex/common/parser.h"
#include "vertex/common/token.h"
#include "vertex/common/statement.h"

#include "kasmlang/ast.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <memory>

bool parse_expression(const std::vector<Token>& tokens, Expression& expression, std::string& error);
bool evaluate(const Expression& tree, std::size_t index, std::uint64_t& value, std::string& error);
class KasmParser : public Parser {
public:
    explicit KasmParser(std::vector<Token> tokens)
        : Parser(std::move(tokens)), expression_(owned_expression_) {}

    explicit KasmParser(std::vector<Token> tokens, Expression& expression)
        : Parser(std::move(tokens)), expression_(expression) {}
    virtual ~KasmParser() = default;
   
    bool number(std::size_t& node, std::string& error);
    bool primary(std::size_t& node, std::string& error);
    bool sum(std::size_t& node, std::string& error);
    bool product(std::size_t& node, std::string& error);
    bool run(std::string& error);
    const Expression& expression() const { return expression_; }
    bool parse_statement(std::unique_ptr<Statement>& statement, std::string& error);
    bool parse_statements(std::vector<std::unique_ptr<Statement>>& statements, std::string& error);

private:
    std::size_t add_node(Node node);

    Expression owned_expression_;
    Expression& expression_;
};