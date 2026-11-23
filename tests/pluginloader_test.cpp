#include <cstdio>
#include <filesystem>
#include <string>
#include "vertex/loaders/cpuloader.h"
#include "vertex/loaders/grammarloader.h"
#include "vertex/loaders/languageloader.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static int failures = 0;
static void check(bool ok, const char* what) {
    std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++failures;
}

static bool isLoaded(const std::string& path) {
#ifdef _WIN32
    return GetModuleHandleA(std::filesystem::path(path).filename().string().c_str()) != nullptr;
#else
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_NOLOAD);
    if (handle) dlclose(handle);
    return handle != nullptr;
#endif
}

// Usage: pluginloader_test <cpu-plugin> <language-plugin> <grammar-plugin> <non-plugin-library>
int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::fprintf(stderr,
                     "Usage: pluginloader_test <cpu-plugin> <language-plugin> <grammar-plugin> <non-plugin-library>\n");
        return 2;
    }
    const std::string cpuPlugin = argv[1];
    const std::string languagePlugin = argv[2];
    const std::string grammarPlugin = argv[3];
    const std::string notPlugin = argv[4];
    std::string error;

    {
        CpuLoader loader;
        check(loader.open(cpuPlugin, error) && isLoaded(cpuPlugin), "open() loads the plugin");
        check(loader.hasInstance() && loader.cpu()->name() == "example", "open() creates the plugin's CPU");
        check(!loader.open(cpuPlugin, error) && loader.hasInstance(), "second open() is rejected and keeps the CPU");
        check(loader.close() && !loader.hasInstance() && !isLoaded(cpuPlugin),
              "close() destroys the CPU and unloads the plugin");
        check(loader.open(cpuPlugin, error) && loader.hasInstance(), "loader can be reopened after close()");
    }
    check(!isLoaded(cpuPlugin), "destructor unloads the plugin");

    {
        CpuLoader loader;
        check(!loader.open(notPlugin, error) && !error.empty() && !loader.hasInstance(),
              "library without plugin exports is rejected");
        check(!loader.open("does_not_exist_plugin", error) && !error.empty(), "missing file is rejected");
        check(loader.open(cpuPlugin, error), "loader is usable after failed opens");
    }

    {
        LanguageLoader languages;
        GrammarLoader grammars;
        check(languages.open(languagePlugin, error) && languages.language()->name() == "example",
              "LanguageLoader creates the plugin's Language");
        check(grammars.open(grammarPlugin, error) && grammars.grammar()->name() == "example",
              "GrammarLoader creates the plugin's Grammar");
    }
    check(!isLoaded(languagePlugin) && !isLoaded(grammarPlugin), "language and grammar plugins are unloaded");

    {
        LanguageLoader loader;
        check(!loader.open(cpuPlugin, error) && error.find("Not a language plugin") == 0 && !isLoaded(cpuPlugin),
              "plugin of a different kind is rejected and unloaded");
    }

    std::printf("%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
