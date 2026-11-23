#pragma once
#include <string>
#include <memory>
#include "registers.h"
class Cpu {
    public:
    Cpu(const std::string& name, std::unique_ptr<RegisterFile> register_file)
        : name_(name), register_file_(std::move(register_file)) {}
    virtual ~Cpu() = default;
    std::string name() const { return name_; }
    RegisterFile* register_file() const { return register_file_.get(); }
    
    private:
    std::string name_;
    std::unique_ptr<RegisterFile> register_file_;
};