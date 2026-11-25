#pragma once
#include "vertex/common/lexer.h"
#include "vertex/common/token.h"
#include <vector>
class KasmLexer : public Lexer {
public:
    using Lexer::Lexer;
     std::vector<Token> tokenize() override;
};