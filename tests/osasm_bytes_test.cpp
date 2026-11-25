#include "kasmlang/grammar.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
    KasmGrammar grammar;
    std::vector<std::unique_ptr<Statement>> statements;
    std::string error;
    struct ByteCase {
        const char* name;
        const char* source;
        std::vector<std::uint8_t> little;
        std::vector<std::uint8_t> big;
    };
    const ByteCase cases[] = {
        {
            "db/dw",
            "db 0xAA\ndw 0x1234, 0xFFFF\ndb 0xBB\n",
            {0xAA, 0x34, 0x12, 0xFF, 0xFF, 0xBB},
            {0xAA, 0x12, 0x34, 0xFF, 0xFF, 0xBB},
        },
        {
            "dd",
            "db 0xAA\ndd 0x12345678, 0xFFFFFFFF, 0\ndb 0xBB\n",
            {0xAA, 0x78, 0x56, 0x34, 0x12, 0xFF, 0xFF, 0xFF, 0xFF,
             0, 0, 0, 0, 0xBB},
            {0xAA, 0x12, 0x34, 0x56, 0x78, 0xFF, 0xFF, 0xFF, 0xFF,
             0, 0, 0, 0, 0xBB},
        },
        {
            "dq",
            "db 0xAA\ndq 0x0123456789ABCDEF, 0xFFFFFFFFFFFFFFFF, 0\ndb 0xBB\n",
            {0xAA, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0, 0, 0, 0, 0, 0, 0, 0, 0xBB},
            {0xAA, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0, 0, 0, 0, 0, 0, 0, 0, 0xBB},
        },
    };
    const Endianness byte_orders[] = {Endianness::Little, Endianness::Big};
    int failures = 0;
    for(const auto& test : cases) {
        grammar.set_source(Source{"bytes.asm", test.source});
        statements.clear();
        if(!grammar.parse_statements(statements, error)) {
            std::cerr << test.name << " parsing failed: " << error << '\n';
            ++failures;
            continue;
        }
        for(const auto order : byte_orders) {
            const char* order_name = order == Endianness::Little ? "Little" : "Big";
            const auto& expected = order == Endianness::Little ? test.little : test.big;
            std::vector<std::uint8_t> output;
            if(!grammar.collect_bytes(statements, output, order, error)) {
                std::cerr << test.name << ' ' << order_name
                          << " collection failed: " << error << '\n';
                ++failures;
                continue;
            }
            if(output != expected) {
                std::cerr << test.name << ' ' << order_name << " produced unexpected bytes\n";
                ++failures;
                continue;
            }
            std::cout << test.name << ' ' << order_name << " bytes:";
            for(const auto byte : output) {
                std::cout << ' ' << static_cast<unsigned>(byte);
            }
            std::cout << '\n';
        }
    }

    const char* invalid_directives[] = {
        "db 256",
        "dw 65536",
        "dd 4294967296",
        "dq 18446744073709551616",
        "dd",
        "dq",
        "dd 1 + 2",
        "dq 1 + 2",
    };
    for(const auto* directive : invalid_directives) {
        grammar.set_source(Source{"invalid-bytes.asm", std::string("db 1\n") + directive + "\n"});
        statements.clear();
        if(!grammar.parse_statements(statements, error)) {
            std::cerr << "Failed to parse invalid fixture " << directive << ": " << error << '\n';
            ++failures;
            continue;
        }
        error.clear();
        if(grammar.analyze(statements, error) || error.empty()) {
            std::cerr << "Semantic analysis did not reject with an error: " << directive << '\n';
            ++failures;
        }
        for(const auto order : byte_orders) {
            const char* order_name = order == Endianness::Little ? "Little" : "Big";
            const std::vector<std::uint8_t> original{0xCC};
            std::vector<std::uint8_t> output = original;
            error.clear();
            if(grammar.collect_bytes(statements, output, order, error)) {
                std::cerr << order_name << " accepted invalid directive: " << directive << '\n';
                ++failures;
                continue;
            }
            if(error.empty() || output != original) {
                std::cerr << order_name << " rejection lacked an error or modified output: "
                          << directive << '\n';
                ++failures;
                continue;
            }
            std::cout << order_name << " rejected " << directive
                      << "; output unchanged: " << error << '\n';
        }
    }

    return failures == 0 ? 0 : 1;
}
