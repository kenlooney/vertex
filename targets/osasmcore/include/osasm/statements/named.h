#pragma once

#include "vertex/common/statement.h"
#include "vertex/common/token.h"

#include <string>
#include <utility>
#include <vector>

class NamedStatement : public Statement
{
public:
    NamedStatement(
        Span span,
        std::string kind,
        std::string name,
        std::vector<Token> operands = {})
        : Statement(span, std::move(kind)),
          name_(std::move(name)),
          operands_(std::move(operands)) {}

    const std::string& name() const { return name_; }
    const std::vector<Token>& operands() const { return operands_; }

private:
    std::string name_;
    std::vector<Token> operands_;
};