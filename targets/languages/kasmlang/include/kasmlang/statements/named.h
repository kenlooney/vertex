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
        std::vector<std::vector<Token>> operands = {})
        : Statement(span, std::move(kind)),
          name_(std::move(name)),
          operands_(std::move(operands)) {}

    const std::string &name() const { return name_; }
    void set_name(const std::string &name) { name_ = name; }
    const std::vector<std::vector<Token>> &operands() const { return operands_; }
    void set_filename(const std::string &filename)
    {
        filename_ = filename;
    }
    const std::string &filename() const
    {
        return filename_;
    }

private:
    std::string name_;
    std::vector<std::vector<Token>> operands_;
    std::string filename_;
};