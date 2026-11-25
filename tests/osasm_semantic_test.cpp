#include "kasmlang/lexer.h"
#include "kasmlang/parser.h"
#include "kasmlang/semantic.h"
#include "kasmlang/statements/directives.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Expected a directive fixture path\n";
        return 1;
    }

    Source source;
    if(!Source::load(argv[1], source)) {
        std::cerr << "Failed to load directive fixture: " << argv[1] << '\n';
        return 1;
    }

    KasmLexer lexer(source);
    KasmParser parser(lexer.tokenize());
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;
    if(!parser.parse_statements(statements, error)) {
        std::cerr << "Failed to parse directives: " << error << '\n';
        return 1;
    }

    KasmSemanticAnalyzer analyzer;
    if(!analyzer.analyze(statements, error)) {
        std::cerr << "Semantic analysis failed: " << error << '\n';
        return 1;
    }

    for(const auto& statement : statements) {
        const auto* directive =
            dynamic_cast<const DirectiveStatement*>(statement.get());
        if(!directive) {
            std::cerr << "Expected a directive statement\n";
            return 1;
        }
        std::cout << "Directive: " << directive->name()
                  << " operands=" << directive->operands().size() << '\n';
    }

    return 0;
}
