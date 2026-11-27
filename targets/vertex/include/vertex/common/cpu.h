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
#include <string>
#include <memory>
#include "registers.h"
#include "encoder.h"
#include "decoder.h"
enum class Endianness
{
    Little,
    Big
};
class Cpu {
    public:
    Cpu(const std::string& name, std::unique_ptr<RegisterFile> register_file, Endianness endianness, std::unique_ptr<Encoder> encoder)
        : name_(name), register_file_(std::move(register_file)), encoder_(std::move(encoder)), endianness_(endianness) {}
    virtual ~Cpu() = default;
    std::string name() const { return name_; }
    RegisterFile* register_file() const { return register_file_.get(); }
    Encoder* encoder() const { return encoder_.get(); }
    virtual Decoder* decoder() { return decoder_.get(); }
    void set_decoder(std::unique_ptr<Decoder> decoder) { decoder_ = std::move(decoder); }
    
    virtual void reset() = 0;
    Endianness endianness() const { return endianness_; }
    private:
    std::string name_;
    std::unique_ptr<RegisterFile> register_file_;
    std::unique_ptr<Encoder> encoder_;
    std::unique_ptr<Decoder> decoder_;

    Endianness endianness_;
};