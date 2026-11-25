#pragma once
#include "vertex/common/semantic.h"

class KasmSemanticAnalyzer : public SemanticAnalyzer
{
public:
    bool analyze(
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::string& error) override;
};
