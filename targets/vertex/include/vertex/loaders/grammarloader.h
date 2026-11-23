#pragma once
#include "vertex/loaders/pluginloader.h"
#include "vertex/plugins/grammarplugin.h"

// Loads a grammar plugin library and owns the Grammar it creates until close().
class GrammarLoader : public PluginLoader<Grammar> {
public:
    Grammar* grammar() const { return get(); }
};
