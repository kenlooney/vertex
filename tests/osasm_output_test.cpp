#include "kasmlang/grammar.h"
#include "osasm/assembler.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

class OutputTestCpu : public Cpu {
public:
    OutputTestCpu() : Cpu("output-test", std::make_unique<RegisterFile>(), Endianness::Little) {}
    void reset() override {}
};

int main() {
    std::error_code error;
    const auto temporary_root = std::filesystem::temp_directory_path(error);
    if(error) {
        std::cerr << "Failed to find temporary directory: " << error.message() << '\n';
        return 1;
    }
    const auto directory = temporary_root / (
        "vertex-output-test-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    if(!std::filesystem::create_directory(directory, error)) {
        std::cerr << "Failed to create test directory: " << error.message() << '\n';
        return 1;
    }
    const auto path = directory / "output.bin";
    OutputTestCpu cpu;
    KasmGrammar grammar;
    OsasmAssembler assembler;
    assembler.setCpu(&cpu);
    assembler.setGrammar(&grammar);

    const auto assemble = [&](const std::string& text, const std::filesystem::path& output_path,
                              const char* format = nullptr) {
        Options options;
        options.add(std::make_unique<StringOption>("input", "output-test.asm"));
        options.add(std::make_unique<StringOption>("output", output_path.string()));
        if(format) {
            options.add(std::make_unique<StringOption>("format", format));
        }
        return assembler.assemble(Source{"output-test.asm", text}, options);
    };
    const auto matches_file = [&](const std::vector<std::uint8_t>& expected) {
        std::ifstream file(path, std::ios::binary);
        if(!file) {
            std::cerr << "Failed to read test output\n";
            return false;
        }
        const std::vector<std::uint8_t> actual{
            std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        if(file.bad() || actual != expected) {
            std::cerr << "Output file did not contain the exact expected bytes\n";
            return false;
        }
        return true;
    };

    int failures = 0;
    const std::string data =
        "db 0, 10, 13, 255\n"
        "dw 0x1234\n"
        "dd 0x12345678\n"
        "dq 0x0102030405060708\n";
    const std::vector<std::uint8_t> expected{
        0, 10, 13, 255, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12,
        8, 7, 6, 5, 4, 3, 2, 1};

    for(const auto& text : {std::string("format raw\n") + data, data}) {
        const auto result = assemble(text, path);
        const auto* details = dynamic_cast<const OsasmAssembleOptionResult*>(result.get());
        if(!result || result->failed() || !details || details->format() != "raw" ||
            !matches_file(expected)) {
            std::cerr << "Raw binary assembly failed\n";
            ++failures;
        }
    }

    for(const auto& text : {std::string("format testformat\n") + data,
                           std::string("db 256\n")}) {
        const auto result = assemble(text, path);
        if(!result || result->succeeded() || !matches_file(expected)) {
            std::cerr << "Invalid assembly succeeded or modified an existing output file\n";
            ++failures;
        }
    }
    const auto conflicting_result = assemble("format raw\n" + data, path, "testformat");
    if(!conflicting_result || conflicting_result->succeeded() || !matches_file(expected)) {
        std::cerr << "Conflicting format modified output or succeeded\n";
        ++failures;
    }

    const auto open_failure = assemble(data, directory);
    if(!open_failure || open_failure->succeeded()) {
        std::cerr << "Opening a directory as an output file did not fail\n";
        ++failures;
    }
    const auto empty_result = assemble("", path);
    if(!empty_result || empty_result->failed() || !matches_file({})) {
        std::cerr << "Empty assembly failed to truncate output to zero bytes\n";
        ++failures;
    }

    std::filesystem::remove_all(directory, error);
    if(error) {
        std::cerr << "Failed to clean test directory: " << error.message() << '\n';
        ++failures;
    }
    if(failures == 0) {
        std::cout << "Raw output: exact bytes, default format, truncation, and failure cases passed\n";
    }
    return failures == 0 ? 0 : 1;
}
