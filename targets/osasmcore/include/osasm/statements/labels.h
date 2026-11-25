#pragma once

#include "osasm/statements/named.h"

class LabelStatement : public NamedStatement
{
public:
    explicit LabelStatement(Span span, std::string name)
        : NamedStatement(span, "label", std::move(name)) {}
};