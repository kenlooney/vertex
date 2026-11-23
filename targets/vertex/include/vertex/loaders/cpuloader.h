#pragma once
#include "vertex/loaders/pluginloader.h"
#include "vertex/plugins/cpuplugin.h"

// Loads a CPU plugin library and owns the Cpu it creates until close().
class CpuLoader : public PluginLoader<Cpu> {
public:
    Cpu* cpu() const { return get(); }
};
