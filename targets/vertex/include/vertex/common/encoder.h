#pragma once
#include "registers.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct RegisterOperand {
    std::string name;
};

struct ImmediateOperand {
    std::uint64_t value;
};

using Operand = std::variant<RegisterOperand, ImmediateOperand>;

class Encoder {
public:
    virtual ~Encoder() = default;

    // Append the instruction bytes on success. Leave bytes untouched on failure.
    virtual bool encode(const RegisterFile& registers,
                        const std::string& mnemonic,
                        const std::vector<Operand>& operands,
                        std::vector<std::uint8_t>& bytes) = 0;
};
