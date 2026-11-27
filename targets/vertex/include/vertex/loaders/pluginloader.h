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
#include "vertex/loader.h"
#include "vertex/plugins/plugin.h"

// Loads a plugin library, checks that it exports the given plugin kind and owns the object it
// creates until close(). Use PluginLoader<T> (or a loader such as CpuLoader) instead of this directly.
class PluginLoaderBase : public Loader {
public:
    ~PluginLoaderBase() override;
    bool open(const std::string& path, std::string& error) override;
    bool close() override;
    bool hasInstance() const { return instance != nullptr; }
protected:
    explicit PluginLoaderBase(const PluginKind& pluginKind) : kind(pluginKind) {}
    void* object() const { return instance; }
private:
    PluginKind kind;
    void* instance = nullptr;
    PluginDestroyFn destroy = nullptr;
};

template <typename T>
class PluginLoader : public PluginLoaderBase {
public:
    PluginLoader() : PluginLoaderBase(PluginTraits<T>::kind) {}
    T* get() const { return static_cast<T*>(object()); }
};
