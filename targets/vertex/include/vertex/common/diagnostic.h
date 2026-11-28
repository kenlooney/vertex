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
#include "span.h"
#include "source.h"
class Diagnostic {
public:
    virtual ~Diagnostic() = default;
    Diagnostic(std::string filename, std::string message, Span span)
        : filename_(std::move(filename)), message_(std::move(message)), span_(std::move(span)) {}
    virtual void report() = 0;
    virtual void report(const Source &source) = 0;
    Diagnostic(const Diagnostic&) = delete;
    Diagnostic& operator=(const Diagnostic&) = delete;
    std::string filename() const { return filename_; }
    std::string message() const { return message_; }
    Span span() const { return span_; }

   
private:
    std::string filename_;
    std::string message_;
    Span span_;

};