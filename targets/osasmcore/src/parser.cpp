#include "osasm/parser.h"
#include "osasm/statements/statements.h"

#include "utils/integer.h"
#include <limits>
#include "vertex/common/token.h"
#include <vector>


std::size_t OsasmParser::add_node(Node node) {
    const std::size_t index = expression_.nodes.size();
    expression_.nodes.push_back(std::move(node));
    return index;
}

bool OsasmParser::number(std::size_t& node, std::string& error)
{
    if(at_end() || peek().kind != "integer") {
        error = "Expected an integer";
        return false;
    }

    const Token& token = peek();
    std::uint64_t value = 0;
    if(!parse_integer(token.text, value)) {
        error = "Invalid integer: " + token.text;
        return false;
    }

    advance();
    node = add_node(Node{"number", value});
    return true;
}

bool OsasmParser::primary(std::size_t& node, std::string& error)
{
    if(!at_end() && peek().kind == "punctuation" && peek().text == "(") {
        advance();

        if(!sum(node, error)) {
            return false;
        }
        if(at_end() || peek().kind != "punctuation" || peek().text != ")") {
            error = "Expected ')'";
            return false;
        }

        advance();
        return true;
    }

    return number(node, error);
}

bool OsasmParser::sum(std::size_t& node, std::string& error)
{
    if(!product(node, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && (peek().text == "+" || peek().text == "-")) {
        const std::string op = peek().text;
        advance();
        std::size_t right = Node::no_child;
        if(!product(right, error)) {
            return false;
        }

        node = add_node(Node{op, 0, node, right});
    }
    return true;
}

bool OsasmParser::product(std::size_t& node, std::string& error)
{
    if(!primary(node, error)) {
        return false;
    }

    while(!at_end() && peek().kind == "punctuation" && (peek().text == "*" || peek().text == "/" || peek().text == "%")) {
        const std::string op = peek().text;
        advance();
        std::size_t right = Node::no_child;
        if(!primary(right, error)) {
            return false;
        }

        node = add_node(Node{op, 0, node, right});
    }
    return true;
}

bool OsasmParser::run(std::string& error)
{
    expression_.nodes.clear();
    expression_.root = Node::no_child;

    std::size_t root = Node::no_child;
    if(!sum(root, error)) {
        return false;
    }
    if(!at_end()) {
        error = "Unexpected token: " + peek().text;
        return false;
    }

    expression_.root = root;
    return true;
}

bool parse_expression(const std::vector<Token>& tokens, Expression& expression, std::string& error) {
    Expression parsed;
    OsasmParser parser(tokens, parsed);
    if(!parser.run(error)) {
        return false;
    }

    expression = std::move(parsed);
    return true;
}

bool evaluate(const Expression& tree, std::size_t index, std::uint64_t& value, std::string& error) {
    if(index >= tree.nodes.size()) {
        error = "Invalid expression node index";
        return false;
    }

    const Node& node = tree.nodes[index];
    if(node.kind == "number") {
        value = node.value;
        return true;
    }

    if(node.kind != "+"
        && node.kind != "-"
        && node.kind != "*"
        && node.kind != "/"
        && node.kind != "%") {
        error = "Unknown node kind: " + node.kind;
        return false;
    }

    std::uint64_t left = 0;
    std::uint64_t right = 0;
    if(!evaluate(tree, node.left, left, error)) {
        return false;
    }
    if(!evaluate(tree, node.right, right, error)) {
        return false;
    }
    if(node.kind == "+") {
        if(left > std::numeric_limits<std::uint64_t>::max() - right) {
            error = "Integer overflow in expression";
            return false;
        }
        value = left + right;
    } else if(node.kind == "-") {
        if(left < right) {
            error = "Integer underflow in expression";
            return false;
        }
        value = left - right;
    } else if(node.kind == "*") {
        if(right != 0 && left > std::numeric_limits<std::uint64_t>::max() / right) {
            error = "Integer overflow in expression";
            return false;
        }
        value = left * right;
    } else if(node.kind == "/") {
        if(right == 0) {
            error = "Division by zero in expression";
            return false;
        }
        value = left / right;
    } else if(node.kind == "%") {
        if(right == 0) {
            error = "Modulo by zero in expression";
            return false;
        }
        value = left % right;
    }
    return true;
}

bool OsasmParser::parse_statement(
    std::unique_ptr<Statement>& statement,
    std::string& error)
{
    if(at_end()) {
        error = "Expected a statement";
        return false;
    }
    const Token first = advance();
    const std::size_t begin = first.span.begin;
    std::size_t end = first.span.end;
    std::vector<std::vector<Token>> operands;
    std::vector<Token> operand;

    while(!at_end() && peek().kind != "newline") {
        const Token token = advance();
        end = token.span.end;
        if(token.kind == "punctuation" && token.text == ",") {
            if(operand.empty()) {
                error = "Expected an operand before ','.";
                return false;
            }
            operands.push_back(std::move(operand));
            operand.clear();
        } else {
            operand.push_back(token);
        }
    }
    if(!operand.empty()) {
        operands.push_back(std::move(operand));
    } else if(!operands.empty()) {
        error = "Statement cannot end with ','.";
        return false;
    }
    if(!at_end()) advance(); // Consume the newline token if present

    if(first.kind == "label") {
        statement = std::make_unique<LabelStatement>(Span::set(begin, end), first.text);
    }
    else if(first.kind == "identifier") {
        statement = std::make_unique<InstructionStatement>(
            Span::set(begin, end),
            first.text,
            std::move(operands));
    }
    else if(first.kind == "directive") {
        statement = std::make_unique<DirectiveStatement>(
            Span::set(begin, end),
            first.text,
            std::move(operands));
    }
    else {
        error = "Unexpected token at start of statement: " + first.text;
        return false;
    }

    return true;
}

bool OsasmParser::parse_statements(std::vector<std::unique_ptr<Statement>>& output, std::string& error) {
    while (!at_end()) {
        while (!at_end() && peek().kind == "newline") advance();
        if (at_end()) break;

        std::unique_ptr<Statement> statement;
        if (!parse_statement(statement, error)) return false;
        output.push_back(std::move(statement));
    }
    return true;
}