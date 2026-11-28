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
#include "vertex/common/grammar.h"
#include "vertex/plugins/plugin.h"

// Bump whenever Grammar's layout or virtual interface changes so stale plugins are rejected.
#define VERTEX_GRAMMAR_PLUGIN_ABI_VERSION 5

VERTEX_DECLARE_PLUGIN_KIND(Grammar, grammar, "grammar", VERTEX_GRAMMAR_PLUGIN_ABI_VERSION)

// Add to one source file of a grammar plugin. GrammarType must derive from Grammar and be default-constructible.
#define VERTEX_REGISTER_GRAMMAR(GrammarType) VERTEX_REGISTER_PLUGIN(grammar, Grammar, GrammarType)
