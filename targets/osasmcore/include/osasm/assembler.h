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
#include "utils/options.h"
#include "vertex/assemble/assembler.h"
#include "osasm/diagnostic.h"
 
class OsasmAssembleOptionResult : public OptionResult
{
public:
    OsasmAssembleOptionResult(std::string inputPath, std::string outputPath, std::string format)
        : inputPath_(std::move(inputPath)), outputPath_(std::move(outputPath)), format_(std::move(format)) {}

    const std::string& inputPath() const { return inputPath_; }
    const std::string& outputPath() const { return outputPath_; }
    const std::string& format() const { return format_; }

private:
    std::string inputPath_;
    std::string outputPath_;
    std::string format_;
};

class OsasmAssembler : public Assembler {
public:
    OsasmAssembler() {}
    virtual ~OsasmAssembler() = default;

    std::unique_ptr<OptionResult> assemble(
        const Source& source,
        const Options& options) override;
};