#pragma once
#include <string>
#include <memory>
#include "registers.h"

enum class Endianness
{
    Little,
    Big
};
class Cpu {
    public:
    Cpu(const std::string& name, std::unique_ptr<RegisterFile> register_file, Endianness endianness)
        : name_(name), register_file_(std::move(register_file)), endianness_(endianness) {}
    virtual ~Cpu() = default;
    std::string name() const { return name_; }
    RegisterFile* register_file() const { return register_file_.get(); }
    virtual void reset() = 0;
    Endianness endianness() const { return endianness_; }
    private:
    std::string name_;
    std::unique_ptr<RegisterFile> register_file_;
    Endianness endianness_;
};