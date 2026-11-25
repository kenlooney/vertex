#include "kasmlang/grammar.h"
#include "vertex/common/source.h"
#include "vertex/common/token.h"

#include <iostream>

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Expected a source fixture path\n";
        return 1;
    }

    KasmGrammar grammar;
    Source source;
    if(!Source::load(argv[1], source)) {
        std::cerr << "Failed to load source file: " << argv[1] << '\n';
        return 1;
    }
    grammar.set_source(source);
    std::vector<Token> tokens = grammar.lexer()->tokenize();
    for (const auto& token : tokens) {
        std::cout << "Token kind: " << token.kind << "('" << token.text << "')" << std::endl;
    }
}