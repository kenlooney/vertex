#pragma once
#include <string>

class Grammar {
    public:
    Grammar(const std::string& name) : name_(name) {}
    virtual ~Grammar() = default;
    std::string name() const { return name_; }
    private:
    std::string name_;
};
