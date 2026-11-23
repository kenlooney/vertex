#include "vertex/loaders/pluginloader.h"

PluginLoaderBase::~PluginLoaderBase() {
    PluginLoaderBase::close();
}

bool PluginLoaderBase::open(const std::string& path, std::string& error) {
    if (!Loader::open(path, error)) return false;

    auto fail = [&](const std::string& message) {
        error = message;
        Loader::close();
        return false;
    };

    auto abiVersion = reinterpret_cast<PluginAbiVersionFn>(symbol(kind.abiVersionSymbol));
    auto create = reinterpret_cast<PluginCreateFn>(symbol(kind.createSymbol));
    auto destroyFn = reinterpret_cast<PluginDestroyFn>(symbol(kind.destroySymbol));
    if (!abiVersion || !create || !destroyFn) {
        return fail(std::string("Not a ") + kind.name + " plugin (missing " + kind.abiVersionSymbol + ", " +
                    kind.createSymbol + " or " + kind.destroySymbol + " export)");
    }

    int version = abiVersion();
    if (version != kind.abiVersion) {
        return fail(std::string(kind.name) + " plugin ABI version " + std::to_string(version) +
                    " does not match host version " + std::to_string(kind.abiVersion));
    }

    void* created = nullptr;
    try {
        created = create();
    } catch (...) {
        created = nullptr;
    }
    if (!created) return fail(std::string(kind.name) + " plugin failed to create an instance");

    instance = created;
    destroy = destroyFn;
    return true;
}

bool PluginLoaderBase::close() {
    bool result = true;
    if (instance && destroy) {
        try {
            destroy(instance);
        } catch (...) {
            result = false;
        }
        instance = nullptr;
    }
    // destroy points into the plugin, so it must not outlive the library.
    destroy = nullptr;
    bool libraryClosed = Loader::close();
    return result && libraryClosed;
}
