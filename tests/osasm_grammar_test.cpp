#include "osasm/grammar.h"
#include "vertex/common/source.h"
#include "vertex/common/token.h"
#include <iostream>
int main(int argc, char** argv) {
    OsasmGrammar grammar;
    Source source;
    Source::load(argv[1], source);
    grammar.set_source(source);
    std::vector<Token> tokens = grammar.lexer()->tokenize();
    for (const auto& token : tokens) {
        std::cout << "Token kind: " << token.kind << std::endl;
    }
}