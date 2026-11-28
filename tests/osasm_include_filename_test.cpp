#include "kasmlang/grammar.h"

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Expected the main source path and the included file path\n";
        return 1;
    }

    Source source;
    if (!Source::load(argv[1], source))
    {
        std::cerr << "Failed to load " << argv[1] << '\n';
        return 1;
    }

    KasmGrammar grammar;
    grammar.set_source(source);
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;
    if (!grammar.parse_statements(statements, error))
    {
        std::cerr << error << '\n';
        return 1;
    }
    if (statements.size() != 3)
    {
        std::cerr << "Expected three statements after include expansion\n";
        return 1;
    }

    const auto *directive = dynamic_cast<const DirectiveStatement *>(statements[1].get());
    if (!directive || directive->name() != "db")
    {
        std::cerr << "Expected the included db directive between the main directives\n";
        return 1;
    }

    std::cout << "Stored filename for included db: " << directive->filename() << '\n';
    const std::filesystem::path actual(directive->filename());
    const std::filesystem::path expected(argv[2]);
    if (actual.lexically_normal() != expected.lexically_normal())
    {
        std::cerr << "Expected filename: " << expected.string() << '\n';
        return 1;
    }
    return 0;
}
