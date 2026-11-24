#pragma once
#include <string>
#include "lexer.h"
#include "parser.h"
#include <memory>
class Grammar {
    public:
    Grammar(const std::string& name) : name_(name) {}
    virtual ~Grammar() = default;
    std::string name() const { return name_; }
    void set_source(const Source& source) {
        lexer_ = make_lexer(source);
        parser_ = make_parser(lexer_->tokenize());
    }
    Lexer* lexer() const { return lexer_.get(); }
    Parser* parser() const { return parser_.get(); }
    private:
    std::string name_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
    protected:
    virtual std::unique_ptr<Lexer> make_lexer(const Source& source) = 0;
    virtual std::unique_ptr<Parser> make_parser(std::vector<Token> tokens) = 0;

};
