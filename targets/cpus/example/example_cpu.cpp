#include "vertex/plugins/cpuplugin.h"
#include "vertex/common/registers.h"
#include <memory>
#include "vertex/common/encoder.h"
class ExampleRegisterFile : public RegisterFile {
public:
    ExampleRegisterFile() {
        addRegister(std::make_unique<Register>("a"));
        addRegister(std::make_unique<Register>("b"));
    }
};
class ExampleEncoder : public Encoder {
public:
    bool encode(const RegisterFile&, const std::string&, const std::vector<Operand>&,
                std::vector<std::uint8_t>&) override {
        return false;
    }
};
class ExampleCpu : public Cpu {
public:
    ExampleCpu() : Cpu("example", std::make_unique<ExampleRegisterFile>(), Endianness::Little, std::make_unique<ExampleEncoder>()) {}
    void reset() override {
        // Reset logic for ExampleCpu
    }
};
VERTEX_REGISTER_CPU(ExampleCpu)
