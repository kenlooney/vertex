#pragma once
#include <memory>
#include "vertex/common/grammar.h"
#include "lexer.h"
#include "parser.h"
#include "kasmlang/semantic.h"
#include "kasmlang/statements/statements.h"
#include "utils/integer.h"
#include <iostream>

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
        set_format("");
        KasmSemanticAnalyzer analyzer;
        if (!analyzer.analyze(statements, error))
        {
            return false;
        }
        set_format(analyzer.format());
        return true;
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

            if (!directive || (
                directive->name() != "db" && 
                directive->name() != "dw" && 
                directive->name() != "dd" &&
                directive->name() != "dq" &&
                directive->name() != "format"))
            {
                error = "Byte collection currently supports only 'db', 'dw', 'dd', 'dq', and 'format' directives";
                return false;
            }

            for (const auto &operand : directive->operands())
            {
                
                if(directive->name() == "dq")
                {
                    std::uint64_t qword = 0;
                    if (!parse_qword(operand.front().text, qword))
                    {
                        error = "Invalid qword value: " + operand.front().text;
                        return false;
                    }
                    if (endianness == Endianness::Little)
                    {
                        bytes.push_back(static_cast<std::uint8_t>(qword & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 8) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 16) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 24) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 32) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 40) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 48) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 56) & 0xFF));
                    }
                    else
                    {
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 56) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 48) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 40) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 32) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 24) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 16) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((qword >> 8) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>(qword & 0xFF));
                    }
                    continue;
                }
                if(directive->name() == "dd")
                {
                    std::uint32_t dword = 0;
                    if (!parse_dword(operand.front().text, dword))
                    {
                        error = "Invalid dword value: " + operand.front().text;
                        return false;
                    }
                    if (endianness == Endianness::Little)
                    {
                        bytes.push_back(static_cast<std::uint8_t>(dword & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 8) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 16) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 24) & 0xFF));
                    }
                    else
                    {
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 24) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 16) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>((dword >> 8) & 0xFF));
                        bytes.push_back(static_cast<std::uint8_t>(dword & 0xFF));
                    }
                    continue;
                }
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
                else if (directive->name() == "format")
                {
                    // No bytes are generated for the format directive
                    if (operand.empty())
                    {
                        error = "The 'format' directive requires a format specifier.";
                        return false;
                    }
                    std::string format = operand.front().text;
                    // For now just print the format specifier
                    std::cout << "Output Format: " << format << std::endl;
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