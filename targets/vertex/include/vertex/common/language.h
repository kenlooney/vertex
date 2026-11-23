#pragma once
#include <string>

class Language {
    public:
    Language(const std::string& name) : name_(name) {}
    virtual ~Language() = default;
    std::string name() const { return name_; }
    private:
    std::string name_;
};
