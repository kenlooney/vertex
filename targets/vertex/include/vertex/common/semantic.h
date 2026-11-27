
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
#include "vertex/common/statement.h"
#include <memory>
#include <string>
#include <vector>

class SemanticAnalyzer {
public:
    SemanticAnalyzer() = default;
    virtual ~SemanticAnalyzer() = default;
    virtual bool analyze(
        const std::vector<std::unique_ptr<Statement>>& statements,
        std::string& error) = 0;
};
