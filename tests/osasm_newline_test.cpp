#include "osasm/lexer.h"
#include "vertex/common/source.h"

#include <iostream>
#include <string>
#include <vector>

int main() {
    Source source{
        "newline.asm",
        "; first\r\nnop\r// second\rhalt\nlast",
    };

    OsasmLexer lexer(source);
    const std::vector<Token> tokens = lexer.tokenize();
    const std::vector<std::string> expected_kinds = {
        "newline",
        "identifier",
        "newline",
        "newline",
        "identifier",
        "newline",
        "identifier",
    };

    if(tokens.size() != expected_kinds.size()) {
        std::cerr << "Unexpected token count\n";
        return 1;
    }

    for(std::size_t index = 0; index < tokens.size(); ++index) {
        if(tokens[index].kind != expected_kinds[index]) {
            std::cerr << "Unexpected token " << index << '\n';
            return 1;
        }
        if(tokens[index].kind == "newline"
            && tokens[index].span.end <= tokens[index].span.begin) {
            std::cerr << "Newline span is empty\n";
            return 1;
        }
    }

    return 0;
}
