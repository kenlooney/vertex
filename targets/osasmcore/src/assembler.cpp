#include "osasm/assembler.h"
#include <cstdint>
#include <vector>

std::unique_ptr<OptionResult> OsasmAssembler::assemble(
    const Source &source,
    const Options &options)
{
    const auto *inputOption =
        dynamic_cast<const StringOption *>(options.find("input"));
    const auto *outputOption =
        dynamic_cast<const StringOption *>(options.find("output"));

    auto result = std::make_unique<OsasmAssembleOptionResult>(
        inputOption ? inputOption->value() : "",
        outputOption ? outputOption->value() : "");
    result->set(false);

    if (!inputOption || !outputOption)
    {
        OsasmDiagnostic diag(
            "<unknown>",
            "Missing input or output option",
            Span{0, 0});
        diag.report();
        return result;
    }

    if (!grammar())
    {
        OsasmDiagnostic diag(
            source.path,
            "No grammar configured",
            Span{0, 0});
        diag.report();
        return result;
    }

    grammar()->set_source(source);
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;

    if (!grammar()->parse_statements(statements, error))
    {
        OsasmDiagnostic diag(source.path, error, Span{0, 0});
        diag.report();
        return result;
    }

    if (!grammar()->analyze(statements, error))
    {
        OsasmDiagnostic diag(source.path, error, Span{0, 0});
        diag.report();
        return result;
    }

    std::vector<std::uint8_t> output;
    
    if(!cpu()) {
        OsasmDiagnostic diag(source.path, "No CPU configured", Span{0, 0});
        diag.report();
        return result;
    }
    if (!grammar()->collect_bytes(statements, output, cpu()->endianness(), error))
    {
        OsasmDiagnostic diag(source.path, error, Span{0, 0});
        diag.report();
        return result;
    }

    return result;
}
