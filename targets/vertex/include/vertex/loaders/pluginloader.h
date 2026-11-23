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
