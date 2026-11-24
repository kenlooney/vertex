#pragma once
#include "token.h"
#include "source.h"
#include <vector>
class Lexer
{
public:
    Lexer(const Source &source)
        : source_(source) {}
    virtual ~Lexer() = default;
    virtual std::vector<Token> tokenize() = 0;
    const Source &source() const { return source_; }
    const std::vector<Token> &get_tokens() const { return tokens_; }
    void add_token(const Token &token) { tokens_.push_back(token); }
    void add_token(std::string kind, std::string text, Span span)
    {
        tokens_.push_back({kind, text, span});
    }
    void clear_tokens() { tokens_.clear(); }
    bool letter(char c) const {
        return std::isalpha(static_cast<unsigned char>(c));
    }
    bool digit(char c) const {
        return std::isdigit(static_cast<unsigned char>(c));
    }
    

private:
    Source source_;
    std::vector<Token> tokens_;
};
