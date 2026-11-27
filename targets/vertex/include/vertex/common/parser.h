
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
#include "vertex/common/token.h"
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>


class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)) {}

    virtual ~Parser() = default;

    const std::vector<Token>& get_tokens() const { return tokens_; }
    std::size_t get_cursor() const { return cursor_; }

    void set_cursor(std::size_t cursor) {
        if(cursor > tokens_.size()) {
            throw std::out_of_range("Parser cursor out of range");
        }
        cursor_ = cursor;
    }

    bool at_end() const {
        return cursor_ >= tokens_.size();
    }

    const Token& peek() const {
        if(at_end()) {
            throw std::out_of_range("Parser peek: cursor out of range");
        }
        return tokens_[cursor_];
    }

    Token advance() {
        if(at_end()) {
            throw std::out_of_range("Parser advance: cursor out of range");
        }
        return tokens_[cursor_++];
    }

private:
    std::vector<Token> tokens_;

protected:
    std::size_t cursor_ = 0;
};