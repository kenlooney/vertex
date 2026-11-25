#pragma once
#include "vertex/common/semantic.h"

class OasmSemanticAnalyzer : public SemanticAnalyzer
{
public:
    bool analyze(
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::string& error) override;
};
