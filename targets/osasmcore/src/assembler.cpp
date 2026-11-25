#include "osasm/assembler.h"

std::unique_ptr<OptionResult> OsasmAssembler::assemble(
    const Source& source,
    const Options& options)
{
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

    OsasmDiagnostic diag(
        "<unknown>",
        "OsasmAssembler::assemble not implemented",
        Span{0, 0});
    diag.report();
    return result;
}
