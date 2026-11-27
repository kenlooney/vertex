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

#include "vertex/plugins/cpuplugin.h"
#include "vertex/common/registers.h"
#include <memory>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include "vertex/common/encoder.h"
#include "vertex/common/decoder.h"
#include <string>

class X86Register : public Register
{
public:
    X86Register(const std::string &name, std::size_t width, std::size_t group_code) : Register(name, width), group_code_(group_code) {}
    std::size_t group_code() const { return group_code_; }

private:
    std::size_t group_code_;
};
class X86RegisterFile : public RegisterFile
{
public:
    X86RegisterFile()
    {
        // A registers
        addRegister(std::make_unique<X86Register>("al", 8, 0));
        addRegister(std::make_unique<X86Register>("ah", 8, 4));
        addRegister(std::make_unique<X86Register>("ax", 16, 0));
        addRegister(std::make_unique<X86Register>("eax", 32, 0));
        addRegister(std::make_unique<X86Register>("rax", 64, 0));

        // C registers
        addRegister(std::make_unique<X86Register>("cl", 8, 1));
        addRegister(std::make_unique<X86Register>("ch", 8, 5));
        addRegister(std::make_unique<X86Register>("cx", 16, 1));
        addRegister(std::make_unique<X86Register>("ecx", 32, 1));
        addRegister(std::make_unique<X86Register>("rcx", 64, 1));

        // D registers
        addRegister(std::make_unique<X86Register>("dl", 8, 2));
        addRegister(std::make_unique<X86Register>("dh", 8, 6));
        addRegister(std::make_unique<X86Register>("dx", 16, 2));
        addRegister(std::make_unique<X86Register>("edx", 32, 2));
        addRegister(std::make_unique<X86Register>("rdx", 64, 2));

        // B registers
        addRegister(std::make_unique<X86Register>("bl", 8, 3));
        addRegister(std::make_unique<X86Register>("bh", 8, 7));
        addRegister(std::make_unique<X86Register>("bx", 16, 3));
        addRegister(std::make_unique<X86Register>("ebx", 32, 3));
        addRegister(std::make_unique<X86Register>("rbx", 64, 3));

        // Stack registers
        addRegister(std::make_unique<X86Register>("sp", 16, 4));
        addRegister(std::make_unique<X86Register>("esp", 32, 4));
        addRegister(std::make_unique<X86Register>("rsp", 64, 4));

        // Base pointer registers
        addRegister(std::make_unique<X86Register>("bp", 16, 5));
        addRegister(std::make_unique<X86Register>("ebp", 32, 5));
        addRegister(std::make_unique<X86Register>("rbp", 64, 5));

        // Source index registers
        addRegister(std::make_unique<X86Register>("si", 16, 6));
        addRegister(std::make_unique<X86Register>("esi", 32, 6));
        addRegister(std::make_unique<X86Register>("rsi", 64, 6));

        // Destination index registers
        addRegister(std::make_unique<X86Register>("di", 16, 7));
        addRegister(std::make_unique<X86Register>("edi", 32, 7));
        addRegister(std::make_unique<X86Register>("rdi", 64, 7));
    }
};

class X86Decoder : public Decoder
{
public:
    bool decode(const RegisterFile &registers, const std::vector<std::uint8_t> &bytes, std::size_t &offset, std::string &text) override
    {
        if (offset >= bytes.size())
        {
            return false;
        }
        std::size_t opcode = bytes[offset];

        if ((opcode & 0xF8) == 0xB0) // MOV reg8, imm8 opcodes range from B0 to B7
        {
            return decode_mov_reg8_imm8(registers, bytes, offset, text);
        }

        switch (opcode)
        {
        case 0x90: // NOP opcode
            return decode_nop(registers, bytes, offset, text);
        case 0xEB: // Short jump opcode
            return decode_short_jump(registers, bytes, offset, text);
        case 0xE9: // Near jump in 16-bit mode
            return decode_near_jump(bytes, offset, text);
        case 0xEA: // Far jump opcode
            return decode_far_jump(bytes, offset, text);
        default:
            return false;
        }
    }

private:
    bool decode_short_jump(const RegisterFile &registers, const std::vector<std::uint8_t> &bytes, std::size_t &offset, std::string &text)
    {
        if (offset + 1 >= bytes.size())
        {
            return false;
        }
        const int raw = bytes[offset + 1];
        const int displacement = raw < 0x80 ? raw : raw - 0x100;
        text = "jmp relative " + std::to_string(displacement);
        offset += 2;
        return true;
    }

    static bool decode_near_jump(const std::vector<std::uint8_t> &bytes,
                                 std::size_t &offset, std::string &text)
    {
        if (bytes.size() - offset < 3)
        {
            return false;
        }

        const auto low = static_cast<std::int32_t>(bytes[offset + 1]);
        const auto high = static_cast<std::int32_t>(bytes[offset + 2]);
        const auto raw = low | (high << 8);
        const auto displacement = raw < 0x8000 ? raw : raw - 0x10000;
        text = "jmp near relative " + std::to_string(displacement);
        offset += 3;
        return true;
    }
    static bool decode_far_jump(const std::vector<std::uint8_t> &bytes,
                                std::size_t &offset, std::string &text)
    {
        if (bytes.size() - offset < 5)
        {
            return false;
        }

        const auto b0 = static_cast<std::int32_t>(bytes[offset + 1]);
        const auto b1 = static_cast<std::int32_t>(bytes[offset + 2]);
        const auto b2 = static_cast<std::int32_t>(bytes[offset + 3]);
        const auto b3 = static_cast<std::int32_t>(bytes[offset + 4]);
        std::size_t off = (b1 * 256) + b0;
        std::size_t seg = (b3 * 256) + b2;
        text = "jmp far " + std::to_string(seg) + ":" + std::to_string(off);
        offset += 5;
        return true;
    }

    bool decode_nop(const RegisterFile &registers, const std::vector<std::uint8_t> &bytes, std::size_t &offset, std::string &text)
    {
        if (offset < bytes.size() && bytes[offset] == 0x90) // NOP opcode
        {
            text = "nop";
            ++offset;
            return true;
        }
        return false;
    }
    bool decode_mov_reg8_imm8(const RegisterFile &registers, const std::vector<std::uint8_t> &bytes, std::size_t &offset, std::string &text)
    {
        if (offset >= bytes.size())
        {
            return false;
        }
        if (offset + 1 >= bytes.size())
        {
            return false;
        }
        const auto reg_code = bytes[offset] & 0x07;
        const auto imm = bytes[offset + 1];
        for (const auto *candidate : registers.getRegisters())
        {
            const auto *reg = dynamic_cast<const X86Register *>(candidate);
            if (reg && reg->getWidth() == 8 && reg->group_code() == reg_code)
            {
                text = "mov " + reg->getName() + ", " + std::to_string(imm);
                offset += 2;
                return true;
            }
        }

        return false;
    }
};

class X86Encoder : public Encoder
{
public:
    bool encode(const RegisterFile &registers,
                const std::string &mnemonic,
                const std::vector<Operand> &operands,
                std::vector<std::uint8_t> &bytes) override
    {
        for (const auto &instruction : instructions_)
        {
            if (mnemonic == instruction.mnemonic)
            {
                return instruction.encode(registers, operands, bytes);
            }
        }
        return false;
    }

private:
    struct RegisterImmediate8
    {
        std::uint8_t register_code;
        std::uint8_t value;
    };

    static std::optional<RegisterImmediate8> get_register_immediate8(
        const RegisterFile &registers, const std::vector<Operand> &operands)
    {
        if (operands.size() != 2)
        {
            return std::nullopt;
        }

        const auto *destination = std::get_if<RegisterOperand>(&operands[0]);
        const auto *immediate = std::get_if<ImmediateOperand>(&operands[1]);
        if (!destination || !immediate || immediate->value > 255)
        {
            return std::nullopt;
        }
        const auto *reg = dynamic_cast<const X86Register *>(
            registers.getRegisterByName(destination->name));
        if (!reg || reg->getWidth() != 8 || reg->group_code() > 7)
        {
            return std::nullopt;
        }

        return RegisterImmediate8{static_cast<std::uint8_t>(reg->group_code()),
                                  static_cast<std::uint8_t>(immediate->value)};
    }

    static bool encode_mov_reg8_imm8(const RegisterFile &registers,
                                     const std::vector<Operand> &operands,
                                     std::vector<std::uint8_t> &bytes)
    {
        const auto form = get_register_immediate8(registers, operands);
        if (!form)
        {
            return false;
        }
        // B0 + rb puts the register code in the opcode.
        const auto opcode = static_cast<std::uint8_t>(0xB0 + form->register_code);
        bytes.insert(bytes.end(), {opcode, form->value});
        return true;
    }

    static bool encode_add_reg8_imm8(const RegisterFile &registers,
                                     const std::vector<Operand> &operands,
                                     std::vector<std::uint8_t> &bytes)
    {
        const auto form = get_register_immediate8(registers, operands);
        if (!form)
        {
            return false;
        }
        // 80 /0 uses ModR/M with mod=11 (register), reg=000, and r/m=register code.
        const auto modrm = static_cast<std::uint8_t>(0xC0 | form->register_code);
        bytes.insert(bytes.end(), {0x80, modrm, form->value});
        return true;
    }

    static bool encode_hlt(const RegisterFile &, const std::vector<Operand> &operands,
                           std::vector<std::uint8_t> &bytes)
    {
        if (!operands.empty())
        {
            return false;
        }
        bytes.push_back(0xF4);
        return true;
    }

    static bool encode_nop(const RegisterFile &, const std::vector<Operand> &operands,
                           std::vector<std::uint8_t> &bytes)
    {
        if (!operands.empty())
        {
            return false;
        }
        bytes.push_back(0x90);
        return true;
    }

    static bool encode_near_jump(const RegisterFile &, const std::vector<Operand> &operands,
                                 std::vector<std::uint8_t> &bytes)
    {
        if (operands.size() != 1)
        {
            return false;
        }
        const auto *relative = std::get_if<RelativeOperand>(&operands.front());
        if (!relative || relative->displacement < -32768 || relative->displacement > 32767)
        {
            return false;
        }
        const auto bits = static_cast<std::uint16_t>(relative->displacement);
        bytes.insert(bytes.end(), {0xE9,
                                   static_cast<std::uint8_t>(bits),
                                   static_cast<std::uint8_t>(bits >> 8)});
        return true;
    }

    static bool encode_short_jump(const RegisterFile &, const std::vector<Operand> &operands,
                                  std::vector<std::uint8_t> &bytes)
    {
        if (operands.size() != 1)
        {
            return false;
        }
        const auto *relative = std::get_if<RelativeOperand>(&operands.front());
        if (!relative || relative->displacement < -128 || relative->displacement > 127)
        {
            return false;
        }
        bytes.insert(bytes.end(), {0xEB, static_cast<std::uint8_t>(relative->displacement)});
        return true;
    }
    static bool encode_far_jump(const RegisterFile &, const std::vector<Operand> &operands,
                                std::vector<std::uint8_t> &bytes)
    {
        if (operands.size() != 1)
        {
            return false;
        }
        const auto *far = std::get_if<FarOperand>(&operands.front());
        if (!far)
            return false;

        bytes.insert(bytes.end(), {0xEA,
                                   static_cast<std::uint8_t>(far->offset % 256),
                                   static_cast<std::uint8_t>(far->offset / 256),
                                   static_cast<std::uint8_t>(far->segment % 256),
                                   static_cast<std::uint8_t>(far->segment / 256)});
        return true;
    }

    struct InstructionEncoding
    {
        std::string_view mnemonic;
        bool (*encode)(const RegisterFile &, const std::vector<Operand> &,
                       std::vector<std::uint8_t> &);
    };

    static constexpr InstructionEncoding instructions_[] = {
        {"mov", encode_mov_reg8_imm8},
        {"add", encode_add_reg8_imm8},
        {"hlt", encode_hlt},
        {"nop", encode_nop},
        {"jmp", encode_short_jump},
        {"jmp near", encode_near_jump},
        {"jmp far", encode_far_jump}};
};
class X86Cpu : public Cpu
{
public:
    X86Cpu() : Cpu("x86", std::make_unique<X86RegisterFile>(), Endianness::Little, std::make_unique<X86Encoder>()) {}
    void reset() override
    {
        // Reset logic for X86Cpu
    }
    Decoder *decoder() override
    {
        if (!Cpu::decoder())
        {
            set_decoder(std::make_unique<X86Decoder>());
        }
        return Cpu::decoder();
    }
};
VERTEX_REGISTER_CPU(X86Cpu)
