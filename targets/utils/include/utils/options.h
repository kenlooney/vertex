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
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>



class OptionResult {
public:
    OptionResult() : result_(false) {}
    virtual ~OptionResult() = default;

    void set(bool result) { result_ = result; }
    bool failed() const { return !result_; }
    bool succeeded() const { return result_; }

private:
    bool result_;
};

class Option {
public:
    explicit Option(std::string name)
        : name_(std::move(name)) {
        if (name_.empty()) {
            throw std::invalid_argument("Option name cannot be empty");
        }
    }

    const std::string& name() const { return name_; }
    virtual ~Option() = default;

private:
    std::string name_;
};

class StringOption : public Option {
public:
    StringOption(std::string name, std::string value)
        : Option(std::move(name)), value_(std::move(value)) {}

    const std::string& value() const { return value_; }

private:
    std::string value_;
};

class Options {
public:
    Options() = default;
    virtual ~Options() = default;

    void add(std::unique_ptr<Option> option) {
        if (!option) {
            throw std::invalid_argument("Option cannot be null");
        }
        options_.push_back(std::move(option));
    }

    const std::vector<std::unique_ptr<Option>>& options() const {
        return options_;
    }

    std::size_t size() const {
        return options_.size();
    }

    bool empty() const {
        return options_.empty();
    }

    void clear() {
        options_.clear();
    }

    std::unique_ptr<Option>& operator[](std::size_t index) {
        return options_.at(index);
    }

    const std::unique_ptr<Option>& operator[](std::size_t index) const {
        return options_.at(index);
    }

    Option* get(std::size_t index) {
        return options_.at(index).get();
    }

    const Option* get(std::size_t index) const {
        return options_.at(index).get();
    }

    Option* find(const std::string_view name) {
        for (auto& option : options_) {
            if (option->name() == name) {
                return option.get();
            }
        }
        return nullptr;
    }

    const Option* find(const std::string_view name) const {
        for (const auto& option : options_) {
            if (option->name() == name) {
                return option.get();
            }
        }
        return nullptr;
    }
private:
    std::vector<std::unique_ptr<Option>> options_;
};

