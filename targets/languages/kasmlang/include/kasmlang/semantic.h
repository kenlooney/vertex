#pragma once
#include "vertex/common/semantic.h"

class KasmSemanticAnalyzer : public SemanticAnalyzer
{
public:
    bool analyze(
        const std::vector<std::unique_ptr<Statement>> &statements,
        std::string &error) override;
    const std::string &format() const { return format_; }
    const std::string &error_filename() const { return error_filename_; }
    Span error_span() const { return error_span_; }

private:
    std::string format_;
    std::string error_filename_;
    Span error_span_{};
};
