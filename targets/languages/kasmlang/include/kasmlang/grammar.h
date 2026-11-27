#pragma once
#include <memory>
#include "vertex/common/grammar.h"
#include "lexer.h"
#include "parser.h"
#include "kasmlang/semantic.h"
#include "kasmlang/statements/statements.h"
#include "utils/integer.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include "vertex/common/symbols.h"
#include "vertex/common/fixup.h"

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
        return collect_bytes(statements, output, endianness, nullptr, error);
    }

    bool collect_bytes(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::vector<std::uint8_t> &output,
        const Cpu &cpu,
        std::string &error) override
    {
        return collect_bytes(statements, output, cpu.endianness(), &cpu, error);
    }

private:
    bool collect_bytes(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::vector<std::uint8_t> &output,
        Endianness endianness,
        const Cpu *cpu,
        std::string &error)
    {
        SymbolTable symbols;
        std::vector<Fixup> fixups;

        if (!analyze(statements, error))
        {
            return false;
        }

        std::vector<std::uint8_t> bytes;

        for (const auto &statement : statements)
        {
            if (const auto *label = dynamic_cast<const LabelStatement *>(statement.get()))
            {
                if (!symbols.addSymbol({SymbolType::Label, label->name(), bytes.size()}))
                {
                    error = "Duplicate label: " + label->name();
                    return false;
                }
                continue;
            }

            if (const auto *instruction = dynamic_cast<const InstructionStatement *>(statement.get()))
            {
                if (instruction->name() == "jmp")
                {
                    if (!collect_jump(*instruction, cpu, bytes, fixups, error))
                    {
                        return false;
                    }
                }
                else if (!collect_instruction(*instruction, cpu, bytes, error))
                {
                    return false;
                }
                continue;
            }

            const auto *directive =
                dynamic_cast<const DirectiveStatement *>(statement.get());

            if (!directive || (directive->name() != "db" &&
                               directive->name() != "dw" &&
                               directive->name() != "dd" &&
                               directive->name() != "dq" &&
                               directive->name() != "format"))
            {
                error = "Unsupported statement for byte collection";
                return false;
            }

            for (const auto &operand : directive->operands())
            {

                if (directive->name() == "dq")
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
                if (directive->name() == "dd")
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

        if (!fixups.empty() && !resolve_jump_fixups(fixups, symbols, *cpu, bytes, error))
        {
            return false;
        }

        output = std::move(bytes);
        return true;
    }

    static bool collect_jump(const InstructionStatement &instruction,
                             const Cpu *cpu,
                             std::vector<std::uint8_t> &bytes,
                             std::vector<Fixup> &fixups,
                             std::string &error)
    {
        const auto &operands = instruction.operands();
        if (operands.size() != 1)
        {
            error = "Expected 'jmp label' or 'jmp near label'";
            return false;
        }
        const auto &tokens = operands.front();
        const bool far = tokens.size() > 1 && tokens.front().text == "far";
        if (far)
        {
            std::uint16_t segment = 0;
            std::uint16_t target_offset = 0;

            if (tokens.size() != 4 || tokens[2].text != ":" ||
                !parse_word(tokens[1].text, segment) ||
                !parse_word(tokens[3].text, target_offset))
            {
                error = "Expected 'jmp far segment:offset' with two 16-bit numbers";
                return false;
            }

            if (!cpu || !cpu->encoder() || !cpu->register_file() ||
                !cpu->encoder()->encode(*cpu->register_file(), "jmp far",
                                        {FarOperand{segment, target_offset}}, bytes))
            {
                error = "The selected CPU cannot encode a far jump";
                return false;
            }

            return true;
        }
        const bool near = tokens.size() == 2 && tokens.front().kind == "identifier" &&
                          tokens.front().text == "near";
        if (tokens.size() != 1 && !near)
        {
            error = "Expected 'jmp label' or 'jmp near label'";
            return false;
        }
        const auto &target = tokens.back();
        // A label name can also be a directive keyword or a suffix-style hex literal.
        if (target.kind == "label" || target.text.empty() ||
            !std::isalpha(static_cast<unsigned char>(target.text.front())))
        {
            error = "Expected a jump target label";
            return false;
        }

        const char *encoder_key = near ? "jmp near" : "jmp";
        const auto start = bytes.size();
        if (!cpu || !cpu->encoder() || !cpu->register_file() ||
            !cpu->encoder()->encode(*cpu->register_file(), encoder_key, {RelativeOperand{0}}, bytes) ||
            bytes.size() == start)
        {
            error = "The selected CPU cannot encode a relative jump";
            return false;
        }
        fixups.push_back({target.text, start, bytes.size(), instruction.span(), encoder_key});
        return true;
    }

    static bool resolve_jump_fixups(const std::vector<Fixup> &fixups,
                                    const SymbolTable &symbols,
                                    const Cpu &cpu,
                                    std::vector<std::uint8_t> &bytes,
                                    std::string &error)
    {
        for (const auto &fixup : fixups)
        {
            const auto *target = symbols.getSymbol(fixup.target);
            if (!target)
            {
                error = "Unknown label: " + fixup.target;
                return false;
            }

            const bool backward = target->offset < fixup.next_offset;
            const auto distance = backward ? fixup.next_offset - target->offset
                                           : target->offset - fixup.next_offset;
            if (distance > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
            {
                error = "Relative jump out of range for label: " + fixup.target;
                return false;
            }
            const auto magnitude = static_cast<std::int64_t>(distance);
            const auto displacement = backward ? -magnitude : magnitude;

            std::vector<std::uint8_t> encoded;
            if (!cpu.encoder()->encode(*cpu.register_file(), fixup.encoder_key,
                                       {RelativeOperand{displacement}}, encoded))
            {
                error = "Relative jump out of range for label: " + fixup.target;
                return false;
            }
            if (encoded.size() != fixup.next_offset - fixup.instruction_offset)
            {
                error = "Relative jump changed size for label: " + fixup.target;
                return false;
            }

            std::copy(encoded.begin(), encoded.end(), bytes.data() + fixup.instruction_offset);
        }
        return true;
    }

    static bool collect_instruction(const InstructionStatement &instruction,
                                    const Cpu *cpu,
                                    std::vector<std::uint8_t> &bytes,
                                    std::string &error)
    {
        if (!cpu || !cpu->encoder() || !cpu->register_file())
        {
            error = "No CPU encoder and register file available for '" + instruction.name() + "'";
            return false;
        }

        const RegisterFile &registers = *cpu->register_file();
        std::vector<Operand> operands;
        operands.reserve(instruction.operands().size());
        for (const auto &tokens : instruction.operands())
        {
            // A bare register name can also look like a suffix-style hex literal (ah, bh, ch, dh).
            if (tokens.size() == 1 &&
                (tokens.front().kind == "identifier" ||
                 (tokens.front().kind == "integer" && registers.getRegisterByName(tokens.front().text))))
            {
                operands.emplace_back(RegisterOperand{tokens.front().text});
                continue;
            }

            Expression expression;
            std::uint64_t value = 0;
            if (!parse_expression(tokens, expression, error) ||
                !evaluate(expression, expression.root, value, error))
            {
                return false;
            }
            operands.emplace_back(ImmediateOperand{value});
        }

        if (!cpu->encoder()->encode(registers, instruction.name(), operands, bytes))
        {
            error = "Unsupported instruction or operands: " + instruction.name();
            return false;
        }
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
