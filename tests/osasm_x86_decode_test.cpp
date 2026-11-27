#include "kasmlang/grammar.h"
#include "osasm/assembler.h"
#include "vertex/loaders/cpuloader.h"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: osasm_x86_decode_test <source> <cpu-plugin> <output>\n";
        return 1;
    }

    Source source;
    if (!Source::load(argv[1], source)) {
        std::cerr << "Could not load source: " << argv[1] << '\n';
        return 1;
    }

    CpuLoader loader;
    std::string error;
    if (!loader.open(argv[2], error)) {
        std::cerr << "Could not load x86 plugin: " << error << '\n';
        return 1;
    }

    auto* cpu = loader.cpu();
    auto* decoder = cpu->decoder();
    const auto* registers = cpu->register_file();
    if (!decoder || !registers) {
        std::cerr << "The x86 plugin must supply a decoder and register file\n";
        return 1;
    }

    KasmGrammar grammar;
    OsasmAssembler assembler;
    assembler.setCpu(cpu);
    assembler.setGrammar(&grammar);

    Options options;
    options.add(std::make_unique<StringOption>("input", argv[1]));
    options.add(std::make_unique<StringOption>("output", argv[3]));
    const auto result = assembler.assemble(source, options);
    if (!result || result->failed()) {
        std::cerr << "Could not assemble " << source.path << '\n';
        return 1;
    }

    std::ifstream input(argv[3], std::ios::binary);
    if (!input) {
        std::cerr << "Could not read binary: " << argv[3] << '\n';
        return 1;
    }
    const std::vector<std::uint8_t> bytes{
        std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    const std::vector<std::uint8_t> expected_bytes{
        0xB0, 0x01, 0xB1, 0x02, 0x90, 0xE9, 0x01, 0x00, 0x90, 0xEB, 0xFA, 0xE9, 0xF7, 0xFF};
    if (input.bad() || bytes != expected_bytes) {
        std::cerr << "Unexpected machine bytes for " << source.path << '\n';
        return 1;
    }

    struct ExpectedInstruction {
        const char* text;
        std::size_t next_offset;
    };
    constexpr ExpectedInstruction expected[] = {
        {"mov al, 1", 2}, {"mov cl, 2", 4}, {"nop", 5}, {"jmp near relative 1", 8},
        {"nop", 9}, {"jmp relative -6", 11}, {"jmp near relative -9", 14}};
    std::size_t offset = 0;
    std::string text;
    for (const auto& instruction : expected) {
        const auto start = offset;
        if (!decoder->decode(*registers, bytes, offset, text)) {
            std::cerr << "Could not decode instruction at byte " << start << '\n';
            return 1;
        }
        std::cout << text << '\n';
        if (text != instruction.text || offset != instruction.next_offset) {
            std::cerr << "At byte " << start << ", expected '" << instruction.text
                      << "' and next offset " << instruction.next_offset
                      << ", got '" << text << "' and next offset " << offset << '\n';
            return 1;
        }
    }

    const auto last_text = text;
    if (offset != bytes.size() || decoder->decode(*registers, bytes, offset, text) ||
        offset != bytes.size() || text != last_text) {
        std::cerr << "Decoding past the last instruction must preserve offset and text\n";
        return 1;
    }

    const std::vector<std::uint8_t> truncated_cases[] = {{0xB0}, {0xE9}, {0xE9, 0x01}};
    for (const auto& truncated : truncated_cases) {
        offset = 0;
        if (decoder->decode(*registers, truncated, offset, text) || offset != 0 || text != last_text) {
            std::cerr << "A truncated instruction must preserve offset and text\n";
            return 1;
        }
    }
}
