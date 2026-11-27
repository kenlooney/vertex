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
#include <type_traits>

// Shared machinery for runtime-loaded plugins. Each plugin kind (CPU, language, grammar, ...)
// is declared once with VERTEX_DECLARE_PLUGIN_KIND and exports its own symbols, so a single
// library may provide several kinds. Plugins exchange C++ objects with the host, so they must
// be built with the same compiler and runtime as vertex.

#ifdef _WIN32
#define VERTEX_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define VERTEX_PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

// If these signatures change, bump the ABI version of every plugin kind.
using PluginAbiVersionFn = int (*)();
using PluginCreateFn = void* (*)();
using PluginDestroyFn = void (*)(void*);

struct PluginKind {
    const char* name;
    int abiVersion;
    const char* abiVersionSymbol;
    const char* createSymbol;
    const char* destroySymbol;
};

// Specialized for each plugin base class by VERTEX_DECLARE_PLUGIN_KIND.
template <typename T>
struct PluginTraits;

// id is the token used in the exported symbol names, e.g. cpu -> vertex_create_cpu.
#define VERTEX_DECLARE_PLUGIN_KIND(BaseType, id, displayName, version)                          \
    template <>                                                                                \
    struct PluginTraits<BaseType> {                                                            \
        static constexpr PluginKind kind{displayName, version, "vertex_" #id "_abi_version",    \
                                         "vertex_create_" #id, "vertex_destroy_" #id};          \
    };

// Use the per-kind wrappers (e.g. VERTEX_REGISTER_CPU) instead of this directly. Objects are
// created and deleted inside the plugin so they are freed by the same runtime that allocated them.
#define VERTEX_REGISTER_PLUGIN(id, BaseType, Type)                                                  \
    static_assert(std::is_base_of<BaseType, Type>::value, #Type " must derive from " #BaseType);    \
    VERTEX_PLUGIN_EXPORT int vertex_##id##_abi_version() { return PluginTraits<BaseType>::kind.abiVersion; } \
    VERTEX_PLUGIN_EXPORT void* vertex_create_##id() {                                               \
        try { return static_cast<BaseType*>(new Type()); } catch (...) { return nullptr; }          \
    }                                                                                               \
    VERTEX_PLUGIN_EXPORT void vertex_destroy_##id(void* object) { delete static_cast<BaseType*>(object); }
