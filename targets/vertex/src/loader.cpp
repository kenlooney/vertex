#include "vertex/loader.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h> 
#endif

Loader::~Loader() {
    // Qualified call: a virtual call to the pure virtual close() from a destructor is undefined behavior.
    Loader::close();
}
bool Loader::open(const std::string& path, std::string& error) {
    error.clear();
    if (library) { error = "Plugin already open"; return false; }
#ifdef _WIN32
    library = LoadLibraryA(path.c_str());
    if (!library) { error = "Could not load plugin (Windows error " + std::to_string(GetLastError()) + ")"; return false; }
    
#else
    library = dlopen(path.c_str(), RTLD_NOW);
    if (!library) { error = dlerror(); return false; }
   
#endif
    
    return true;
}
bool Loader::close() {
    if (library) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(library));
#else
        dlclose(library);
#endif
        library = nullptr;
    }
    return true;
}

void* Loader::symbol(const std::string& name) const {
    if (!library) return nullptr;
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(library), name.c_str()));
#else
    return dlsym(library, name.c_str());
#endif
}
