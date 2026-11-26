#pragma once
#include <string>
#include <memory>
#include "registers.h"
#include "encoder.h"
enum class Endianness
{
    Little,
    Big
};
class Cpu {
    public:
    Cpu(const std::string& name, std::unique_ptr<RegisterFile> register_file, Endianness endianness, std::unique_ptr<Encoder> encoder)
        : name_(name), register_file_(std::move(register_file)), encoder_(std::move(encoder)), endianness_(endianness) {}
    virtual ~Cpu() = default;
    std::string name() const { return name_; }
    RegisterFile* register_file() const { return register_file_.get(); }
    Encoder* encoder() const { return encoder_.get(); }

    virtual void reset() = 0;
    Endianness endianness() const { return endianness_; }
    private:
    std::string name_;
    std::unique_ptr<RegisterFile> register_file_;
    std::unique_ptr<Encoder> encoder_;

    Endianness endianness_;
};