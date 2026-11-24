#pragma once
#include <memory>
#include "vertex/common/grammar.h"
#include "osasm/lexer.h"
#include "osasm/parser.h"

class OsasmGrammar : public Grammar {
public:
    OsasmGrammar() : Grammar("osasm") {}

protected:
    std::unique_ptr<Lexer> make_lexer(const Source& source) override {
        return std::make_unique<OsasmLexer>(source);
    }

    std::unique_ptr<Parser> make_parser(std::vector<Token> tokens) override {
        return std::make_unique<OsasmParser>(std::move(tokens));
    }
};