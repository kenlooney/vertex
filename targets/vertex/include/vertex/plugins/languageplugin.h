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
#include "vertex/common/language.h"
#include "vertex/plugins/plugin.h"

// Bump whenever Language's layout or virtual interface changes so stale plugins are rejected.
#define VERTEX_LANGUAGE_PLUGIN_ABI_VERSION 1

VERTEX_DECLARE_PLUGIN_KIND(Language, language, "language", VERTEX_LANGUAGE_PLUGIN_ABI_VERSION)

// Add to one source file of a language plugin. LanguageType must derive from Language and be default-constructible.
#define VERTEX_REGISTER_LANGUAGE(LanguageType) VERTEX_REGISTER_PLUGIN(language, Language, LanguageType)
