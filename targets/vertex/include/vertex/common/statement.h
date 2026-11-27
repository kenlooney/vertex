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
#include "span.h"
#include <string_view>
#include <string>
class Statement
{
public:
    explicit Statement(Span span, std::string kind) : span_(span), kind_(std::move(kind)) {}
    virtual ~Statement() = default; 
    Span span() const { return span_; }
    virtual std::string_view kind() const { return kind_; }
private:
    Span span_;
    std::string kind_;
};