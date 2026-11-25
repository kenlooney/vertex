#pragma once
#include <memory>
#include "utils/options.h"
#include "vertex/assemble/assembler.h"
#include "osasm/diagnostic.h"
 
class OsasmAssembleOptionResult : public OptionResult
{
public:
    OsasmAssembleOptionResult(std::string inputPath, std::string outputPath, std::string format)
        : inputPath_(std::move(inputPath)), outputPath_(std::move(outputPath)), format_(std::move(format)) {}

    const std::string& inputPath() const { return inputPath_; }
    const std::string& outputPath() const { return outputPath_; }
    const std::string& format() const { return format_; }

private:
    std::string inputPath_;
    std::string outputPath_;
    std::string format_;
};

class OsasmAssembler : public Assembler {
public:
    OsasmAssembler() {}
    virtual ~OsasmAssembler() = default;

    std::unique_ptr<OptionResult> assemble(
        const Source& source,
        const Options& options) override;
};