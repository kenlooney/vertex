#include "kasmlang/lexer.h"
#include "kasmlang/parser.h"
#include "kasmlang/statements/instructions.h"
#include "kasmlang/statements/labels.h"
#include "kasmlang/statements/named.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cerr << "Expected a statement fixture path\n";
        return 1;
    }

    Source source;
    if(!Source::load(argv[1], source)) {
        std::cerr << "Failed to load statement fixture: " << argv[1] << '\n';
        return 1;
    }

    KasmLexer lexer(source);
    KasmParser parser(lexer.tokenize());
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;

    if(!parser.parse_statements(statements, error) || statements.size() != 3) {
        std::cerr << "Failed to parse statements: " << error << '\n';
        return 1;
    }

    const auto* label = dynamic_cast<LabelStatement*>(statements[0].get());
    const auto* mov = dynamic_cast<InstructionStatement*>(statements[1].get());
    const auto* nop = dynamic_cast<InstructionStatement*>(statements[2].get());

    if(!label || label->name() != "start") {
        std::cerr << "Failed to parse label statement\n";
        return 1;
    }
    if(!mov || mov->name() != "mov" || mov->operands().size() != 2) {
        std::cerr << "Failed to parse mov operands\n";
        return 1;
    }
    if(!nop || nop->name() != "nop" || !nop->operands().empty()) {
        std::cerr << "Failed to parse nop statement\n";
        return 1;
    }

    for(const auto& statement : statements) {
        const auto* named = dynamic_cast<NamedStatement*>(statement.get());
        std::cout << "Statement kind: " << statement->kind()
                  << "('" << named->name() << "') operands="
                  << named->operands().size() << '\n';
    }
    std::cout << "Parsed statement count: " << statements.size() << '\n';

    return 0;
}
