
#pragma once
#include <string>

class Loader {
    public:
    Loader() {}
    Loader(const Loader&) = delete;
    Loader& operator=(const Loader&) = delete;
    virtual ~Loader();
    virtual bool open(const std::string& path, std::string& error);
    virtual bool close() = 0;
    protected:
    // Returns the address of an exported symbol, or nullptr if no library is open or the symbol is missing.
    void* symbol(const std::string& name) const;
    private:
   
    void* library = nullptr;
};