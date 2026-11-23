#pragma once
#include "vertex/loaders/pluginloader.h"
#include "vertex/plugins/languageplugin.h"

// Loads a language plugin library and owns the Language it creates until close().
class LanguageLoader : public PluginLoader<Language> {
public:
    Language* language() const { return get(); }
};
