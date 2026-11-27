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
#include <memory>

#include "vertex/common/source.h"
#include "utils/options.h"
#include "vertex/common/cpu.h"
#include "vertex/common/grammar.h"

class Assembler
{
public:
    Assembler() = default;
    virtual ~Assembler() = default;
    virtual std::unique_ptr<OptionResult> assemble(
        const Source &source,
        const Options &options) = 0;

    void setCpu(Cpu *cpu)
    {
        cpu_ = cpu;
    }

    void setGrammar(Grammar *grammar)
    {
        grammar_ = grammar;
    }

protected:
    Cpu *cpu() const { return cpu_; }
    Grammar *grammar() const { return grammar_; }

private:
    Cpu *cpu_ = nullptr;
    Grammar *grammar_ = nullptr;
};