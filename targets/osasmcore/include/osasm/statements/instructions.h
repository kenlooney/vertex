#pragma once

#include "osasm/statements/named.h"

class InstructionStatement : public NamedStatement
{
public:
    InstructionStatement(
        Span span,
        std::string opcode,
        std::vector<std::vector<Token>> operands)
        : NamedStatement(
              span,
              "instruction",
              std::move(opcode),
              std::move(operands)) {}
};

