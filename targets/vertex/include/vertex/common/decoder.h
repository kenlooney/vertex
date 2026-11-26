#pragma once
#include "registers.h"
class Decoder {
    public:
        Decoder() = default;
        virtual ~Decoder() = default;
        virtual bool decode(const RegisterFile &registers,const std::vector<std::uint8_t> &bytes, std::size_t& offset, std::string& text) = 0;
        RegisterFile* register_file() const { return register_file_; }
        void set_register_file(RegisterFile* rf) { register_file_ = rf; }
    protected:
        RegisterFile* register_file_ = nullptr;
};