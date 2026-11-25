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
        Endianness endianness,
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

            if (!directive || (directive->name() != "db" && directive->name() != "dw"))
            {
                error = "Byte collection currently supports only 'db' and 'dw' directives";
                return false;
            }

            for (const auto &operand : directive->operands())
            {
                if (directive->name() == "dw")
                {
                    std::uint16_t word = 0;
                    if (!parse_word(operand.front().text, word))
                    {
                        error = "Invalid word value: " + operand.front().text;
                        return false;
                    }
                   
                    if (endianness == Endianness::Little)
                    {
                        bytes.push_back(static_cast<std::uint8_t>(word & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((word >> 8) & 0xFF));
                    }
                    else
                    {
                        bytes.push_back(static_cast<std::uint8_t>((word >> 8) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>(word & 0xFF));
                    }
                    continue;
                }
                else if (directive->name() == "db")
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