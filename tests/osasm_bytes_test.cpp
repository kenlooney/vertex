#include "kasmlang/grammar.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
    const Source source{
        "bytes.asm",
        "db 0xAA\n"
        "dw 0x1234, 0xFFFF\n"
        "db 0xBB\n",
    };
    KasmGrammar grammar;
    grammar.set_source(source);
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;
    if(!grammar.parse_statements(statements, error)) {
        std::cerr << "Failed to parse byte fixture: " << error << '\n';
        return 1;
    }

    struct ByteCase {
        const char* name;
        Endianness endianness;
        std::vector<std::uint8_t> expected;
    };
    const ByteCase cases[] = {
        {"Little endian", Endianness::Little, {0xAA, 0x34, 0x12, 0xFF, 0xFF, 0xBB}},
        {"Big endian", Endianness::Big, {0xAA, 0x12, 0x34, 0xFF, 0xFF, 0xBB}},
    };

    for(const auto& test : cases) {
        std::vector<std::uint8_t> output;
        if(!grammar.collect_bytes(statements, output, test.endianness, error)) {
            std::cerr << test.name << " collection failed: " << error << '\n';
            return 1;
        }
        if(output != test.expected) {
            std::cerr << test.name << " produced unexpected bytes\n";
            return 1;
        }

        std::cout << test.name << " bytes:";
        for(const auto byte : output) {
            std::cout << ' ' << static_cast<unsigned>(byte);
        }
        std::cout << '\n';
    }

    const Source overflow_source{
        "word-overflow.asm",
        "db 1\n"
        "dw 65536\n",
    };
    grammar.set_source(overflow_source);
    statements.clear();
    if(!grammar.parse_statements(statements, error)) {
        std::cerr << "Failed to parse word-overflow fixture: " << error << '\n';
        return 1;
    }

    for(const auto& test : cases) {
        const std::vector<std::uint8_t> original{0xCC};
        std::vector<std::uint8_t> output = original;
        error.clear();
        if(grammar.collect_bytes(statements, output, test.endianness, error)) {
            std::cerr << test.name << " accepted an overflowing word\n";
            return 1;
        }
        if(error.empty()) {
            std::cerr << test.name << " rejected overflow without an error\n";
            return 1;
        }
        if(output != original) {
            std::cerr << test.name << " modified output after overflow\n";
            return 1;
        }
        std::cout << test.name
                  << " rejected dw 65536; output unchanged: " << error << '\n';
    }

    return 0;
}
