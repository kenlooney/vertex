#pragma once
#include "vertex/common/language.h"
#include "vertex/plugins/plugin.h"

// Bump whenever Language's layout or virtual interface changes so stale plugins are rejected.
#define VERTEX_LANGUAGE_PLUGIN_ABI_VERSION 1

VERTEX_DECLARE_PLUGIN_KIND(Language, language, "language", VERTEX_LANGUAGE_PLUGIN_ABI_VERSION)

// Add to one source file of a language plugin. LanguageType must derive from Language and be default-constructible.
#define VERTEX_REGISTER_LANGUAGE(LanguageType) VERTEX_REGISTER_PLUGIN(language, Language, LanguageType)
