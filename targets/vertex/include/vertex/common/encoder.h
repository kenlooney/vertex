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
#include "registers.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

struct RegisterOperand {
    std::string name;
};

struct ImmediateOperand {
    std::uint64_t value;
};

struct RelativeOperand {
    std::int64_t displacement;
};

struct FarOperand {
    std::uint16_t segment;
    std::uint16_t offset;
};

using Operand = std::variant<RegisterOperand, ImmediateOperand, RelativeOperand, FarOperand>;

class Encoder {
public:
    virtual ~Encoder() = default;

    // Append the instruction bytes on success. Leave bytes untouched on failure.
    virtual bool encode(const RegisterFile& registers,
                        const std::string& mnemonic,
                        const std::vector<Operand>& operands,
                        std::vector<std::uint8_t>& bytes) = 0;
};
