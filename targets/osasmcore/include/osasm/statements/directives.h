#pragma once

#include "osasm/statements/named.h"

class DirectiveStatement : public NamedStatement
{
public:
    DirectiveStatement(
        Span span,
        std::string name,
        std::vector<Token> operands)
        : NamedStatement(
              span,
              "directive",
              std::move(name),
              std::move(operands)) {}
};