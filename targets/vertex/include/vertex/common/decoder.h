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
class Decoder {
    public:
        Decoder() = default;
        virtual ~Decoder() = default;
        virtual bool decode(const RegisterFile &registers,const std::vector<std::uint8_t> &bytes, std::size_t& offset, std::string& text) = 0;
        RegisterFile* register_file() const { return register_file_; }
        void set_register_file(RegisterFile* rf) { register_file_ = rf; }
    protected:
        RegisterFile* register_file_ = nullptr;
};