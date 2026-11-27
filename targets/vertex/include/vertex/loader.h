
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

class Loader {
    public:
    Loader() {}
    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;
    virtual ~Loader();
    virtual bool open(const std::string& path, std::string& error);
    virtual bool close() = 0;
    protected:
    // Returns the address of an exported symbol, or nullptr if no library is open or the symbol is missing.
    void* symbol(const std::string& name) const;
    private:
   
    void* library = nullptr;
};