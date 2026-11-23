#pragma once
#include "vertex/common/lexer.h"
#include "vertex/common/token.h"
#include <vector>
class OsasmLexer : public Lexer {
public:
    using Lexer::Lexer;
     std::vector<Token> tokenize() override;
};