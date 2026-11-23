#include "vertex/plugins/cpuplugin.h"
#include "vertex/common/registers.h"
#include <memory>
class ExampleRegisterFile : public RegisterFile {
public:
    ExampleRegisterFile() {
        addRegister(std::make_unique<Register>("a"));
        addRegister(std::make_unique<Register>("b"));
    }
};
class ExampleCpu : public Cpu {
public:
    ExampleCpu() : Cpu("example", std::make_unique<ExampleRegisterFile>()) {}
};
VERTEX_REGISTER_CPU(ExampleCpu)
