#pragma once
#include <string>
#include "lexer.h"
#include "parser.h"
#include <memory>
#include "statement.h"
#include <cstdint>
#include <vector>
#include "cpu.h"
class Grammar
{
public:
    Grammar(const std::string &name) : name_(name) {}
    virtual ~Grammar() = default;
    std::string name() const { return name_; }
    void set_source(const Source &source)
    {
        lexer_ = make_lexer(source);
        parser_ = make_parser(lexer_->tokenize());
    }
    Lexer *lexer() const { return lexer_.get(); }
    Parser *parser() const { return parser_.get(); }
    virtual bool parse_statements(
        std::vector<std::unique_ptr<Statement>> &statements,
        std::string &error) = 0;
    virtual bool analyze(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::string &error) = 0;
    virtual bool collect_bytes(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::vector<std::uint8_t> &output,
        Endianness endianness,
        std::string &error) = 0;

private:
    std::string name_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;

protected:
    virtual std::unique_ptr<Lexer> make_lexer(const Source &source) = 0;
    virtual std::unique_ptr<Parser> make_parser(std::vector<Token> tokens) = 0;
};
