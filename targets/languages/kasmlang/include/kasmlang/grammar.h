#pragma once
#include <memory>
#include "vertex/common/grammar.h"
#include "lexer.h"
#include "parser.h"

class KasmGrammar : public Grammar {
public:
    KasmGrammar() : Grammar("kasmlang") {}

protected:
    std::unique_ptr<Lexer> make_lexer(const Source& source) override {
        return std::make_unique<KasmLexer>(source);
    }

    std::unique_ptr<Parser> make_parser(std::vector<Token> tokens) override {
        return std::make_unique<KasmParser>(std::move(tokens));
    }
};