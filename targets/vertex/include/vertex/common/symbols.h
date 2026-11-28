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
#include <vector>

enum class SymbolType
{
    Label
};

struct Symbol
{
    SymbolType type;
    std::string name;
    std::size_t offset;
    std::string filename;
};

class SymbolTable
{
public:
    bool addSymbol(const Symbol &symbol)
    {
        if (hasSymbol(symbol.name))
            return false;

        symbols.push_back(symbol);
        return true;
    }
    bool hasSymbol(const std::string &name) const
    {
       return getSymbol(name) != nullptr;
    }

    const std::vector<Symbol> &getSymbols() const
    {
        return symbols;
    }
    const Symbol *getSymbol(const std::string &name) const
    {
        for (const auto &symbol : symbols)
        {
            if (symbol.name == name)
                return &symbol;
        }
        return nullptr;
    }

private:
    std::vector<Symbol> symbols;
};