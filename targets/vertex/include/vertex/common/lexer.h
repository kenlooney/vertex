/*
 * Copyright (C) 2026 Kenneth Looney
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#pragma once
#include "token.h"
#include "source.h"
#include <vector>
class Lexer
{
public:
    Lexer(const Source &source)
        : source_(source) {}
    virtual ~Lexer() = default;
    virtual std::vector<Token> tokenize() = 0;
    const Source &source() const { return source_; }
    const std::vector<Token> &get_tokens() const { return tokens_; }
    void add_token(const Token &token) { tokens_.push_back(token); }
    void add_token(std::string kind, std::string text, Span span)
    {
        tokens_.push_back({kind, text, span});
    }
    void clear_tokens() { tokens_.clear(); }
    bool letter(char c) const {
        return std::isalpha(static_cast<unsigned char>(c));
    }
    bool digit(char c) const {
        return std::isdigit(static_cast<unsigned char>(c));
    }
    

private:
    Source source_;
    std::vector<Token> tokens_;
};
