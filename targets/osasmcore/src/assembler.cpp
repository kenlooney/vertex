#include "osasm/assembler.h"
#include <cstdint>
#include <fstream>
#include <limits>
#include <vector>

std::unique_ptr<OptionResult> OsasmAssembler::assemble(
    const Source &source,
    const Options &options)
{
    const auto *inputOption =
        dynamic_cast<const StringOption *>(options.find("input"));
    const auto *outputOption =
        dynamic_cast<const StringOption *>(options.find("output"));
    const auto *formatOption =
        dynamic_cast<const StringOption *>(options.find("format"));

    auto result = std::make_unique<OsasmAssembleOptionResult>(
        inputOption ? inputOption->value() : "",
        outputOption ? outputOption->value() : "",
        formatOption ? formatOption->value() : "");
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

    if (!cpu())
    {
        OsasmDiagnostic diag(source.path, "No CPU configured", Span{0, 0});
        diag.report();
        return result;
    }

    std::string selectedFormat = grammar()->format();
    if (formatOption)
    {
        if (formatOption->value().empty() ||
            (!selectedFormat.empty() && selectedFormat != formatOption->value()))
        {
            OsasmDiagnostic diag(source.path, "Empty or conflicting output format option", Span{0, 0});
            diag.report();
            return result;
        }
        selectedFormat = formatOption->value();
    }
    if (selectedFormat.empty())
    {
        selectedFormat = "raw";
    }

    if (selectedFormat != "raw")
    {
        OsasmDiagnostic diag(source.path, "Unsupported output format: " + selectedFormat, Span{0, 0});
        diag.report();
        return result;
    }

    if (!grammar()->collect_bytes(statements, output, *cpu(), error))
    {
        OsasmDiagnostic diag(source.path, error, Span{0, 0});
        diag.report();
        return result;
    }

    if (output.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
    {
        OsasmDiagnostic diag(outputOption->value(), "Output is too large to write", Span{0, 0});
        diag.report();
        return result;
    }

    std::ofstream file(outputOption->value(), std::ios::binary | std::ios::trunc);
    if (!file)
    {
        OsasmDiagnostic diag(outputOption->value(), "Failed to open output file", Span{0, 0});
        diag.report();
        return result;
    }
    if (!output.empty())
    {
        file.write(reinterpret_cast<const char*>(output.data()),
                   static_cast<std::streamsize>(output.size()));
    }
    if (!file)
    {
        OsasmDiagnostic diag(outputOption->value(), "Failed to write output file", Span{0, 0});
        diag.report();
        return result;
    }
    file.close();
    if (!file)
    {
        OsasmDiagnostic diag(outputOption->value(), "Failed to close output file", Span{0, 0});
        diag.report();
        return result;
    }

    result = std::make_unique<OsasmAssembleOptionResult>(
        inputOption->value(), outputOption->value(), selectedFormat);
    result->set(true);
    return result;
}
