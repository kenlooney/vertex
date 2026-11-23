#pragma once
#include <string>

class Cpu {
    public:
    Cpu(const std::string& name) : name_(name) {}
    virtual ~Cpu() = default;
    std::string name() const { return name_; }
    private:
    std::string name_;
};