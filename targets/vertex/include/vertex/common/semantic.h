#pragma once

#include "vertex/common/statement.h"
#include <memory>
#include <string>
#include <vector>

class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    virtual ~SemanticAnalyzer() = default;
    virtual bool analyze(
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::string& error) = 0;
};
