#include "osasm/grammar.h"
#include "osasm/diagnostic.h"
#include "vertex/common/source.h"
#include "vertex/common/token.h"
#include <iostream>
int main(int argc, char** argv) {
    OsasmGrammar grammar;
    Source source;
    if(!Source::load(argv[1], source)) {
        OsasmDiagnostic diagnostic = OsasmDiagnostic("<unknown>", "Failed to load source file", Span::set(0, 0));
        return 1;
    }
    grammar.set_source(source);
    std::vector<Token> tokens = grammar.lexer()->tokenize();
    for (const auto& token : tokens) {
        std::cout << "Token kind: " << token.kind << "('" << token.text << "')" << std::endl;
    }
}