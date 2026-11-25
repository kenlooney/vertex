#pragma once

#include "kasmlang/statements/named.h"

class DirectiveStatement : public NamedStatement
{
public:
    DirectiveStatement(
        Span span,
        std::string name,
        std::vector<std::vector<Token>> operands)
        : NamedStatement(
              span,
              "directive",
              std::move(name),
              std::move(operands)) {}
};