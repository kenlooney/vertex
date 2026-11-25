#pragma once
#include <memory>
#include "utils/options.h"
#include "vertex/assemble/assembler.h"
#include "osasm/diagnostic.h"
 
class OsasmAssembleOptionResult : public OptionResult
{
public:
    OsasmAssembleOptionResult(std::string inputPath, std::string outputPath)
        : inputPath_(std::move(inputPath)), outputPath_(std::move(outputPath)) {}

    const std::string& inputPath() const { return inputPath_; }
    const std::string& outputPath() const { return outputPath_; }

private:
    std::string inputPath_;
    std::string outputPath_;
};

class OsasmAssembler : public Assembler {
public:
    OsasmAssembler() {}
    virtual ~OsasmAssembler() = default;

    std::unique_ptr<OptionResult> assemble(
        const Source& source,
        const Options& options) override {
        const auto* inputOption =
            dynamic_cast<const StringOption*>(options.find("input"));
        const auto* outputOption =
            dynamic_cast<const StringOption*>(options.find("output"));

        auto result = std::make_unique<OsasmAssembleOptionResult>(
            inputOption ? inputOption->value() : "",
            outputOption ? outputOption->value() : "");
        result->set(false);

        if(!inputOption || !outputOption) {
            OsasmDiagnostic diag(
                "<unknown>",
                "Missing input or output option",
                Span{0, 0});
            diag.report();
            return result;
        }

        OsasmDiagnostic diag("<unknown>","OsasmAssembler::assemble not implemented", Span{0,0});
        diag.report();
        return result;
    }
};