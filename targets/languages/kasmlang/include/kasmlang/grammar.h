#pragma once
#include <memory>
#include "vertex/common/grammar.h"
#include "lexer.h"
#include "parser.h"
#include "kasmlang/semantic.h"
#include "kasmlang/statements/statements.h"
#include "utils/integer.h"

class KasmGrammar : public Grammar
{
public:
    KasmGrammar() : Grammar("kasmlang") {}
    bool parse_statements(
        std::vector<std::unique_ptr<Statement>> &statements,
        std::string &error) override
    {
        auto *kasmParser = dynamic_cast<KasmParser *>(parser());
        if (!kasmParser)
        {
            error = "KASM parser is not initialized";
            return false;
        }

        return kasmParser->parse_statements(statements, error);
    }
    bool analyze(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::string &error) override
    {
        KasmSemanticAnalyzer analyzer;
        return analyzer.analyze(statements, error);
    }
    bool collect_bytes(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::vector<std::uint8_t> &output,
        std::string &error) override
    {
        if (!analyze(statements, error))
        {
            return false;
        }

        std::vector<std::uint8_t> bytes;

        for (const auto &statement : statements)
        {
            const auto *directive =
                dynamic_cast<const DirectiveStatement *>(statement.get());

            if (!directive || directive->name() != "db")
            {
                error = "Byte collection currently supports only 'db' directives";
                return false;
            }

            for (const auto &operand : directive->operands())
            {
                std::uint8_t byte = 0;
                if (!parse_byte(operand.front().text, byte))
                {
                    error = "Invalid byte value: " + operand.front().text;
                    return false;
                }
                bytes.push_back(byte);
            }
        }

        output = std::move(bytes);
        return true;
    }

protected:
    std::unique_ptr<Lexer> make_lexer(const Source &source) override
    {
        return std::make_unique<KasmLexer>(source);
    }

    std::unique_ptr<Parser> make_parser(std::vector<Token> tokens) override
    {
        return std::make_unique<KasmParser>(std::move(tokens));
    }
};