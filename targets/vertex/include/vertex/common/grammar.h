#pragma once
#include <string>
#include "lexer.h"
#include <memory>
class Grammar {
    public:
    Grammar(const std::string& name) : name_(name) {}
    virtual ~Grammar() = default;
    std::string name() const { return name_; }
    void set_source(const Source& source) {
        lexer_ = make_lexer(source);
    }
    Lexer* lexer() const { return lexer_.get(); }
    private:
    std::string name_;
    std::unique_ptr<Lexer> lexer_;
    protected:
    virtual std::unique_ptr<Lexer> make_lexer(const Source& source) = 0;

};
