#pragma once
#include "vertex/common/grammar.h"
#include "vertex/plugins/plugin.h"

// Bump whenever Grammar's layout or virtual interface changes so stale plugins are rejected.
#define VERTEX_GRAMMAR_PLUGIN_ABI_VERSION 2

VERTEX_DECLARE_PLUGIN_KIND(Grammar, grammar, "grammar", VERTEX_GRAMMAR_PLUGIN_ABI_VERSION)

// Add to one source file of a grammar plugin. GrammarType must derive from Grammar and be default-constructible.
#define VERTEX_REGISTER_GRAMMAR(GrammarType) VERTEX_REGISTER_PLUGIN(grammar, Grammar, GrammarType)
