#include "vertex/plugins/cpuplugin.h"
#include "vertex/common/registers.h"
#include <memory>
class X86RegisterFile : public RegisterFile {
public:
    X86RegisterFile() {
        addRegister(std::make_unique<Register>("rax"));
        addRegister(std::make_unique<Register>("rbx"));
    }
};
class X86Cpu : public Cpu {
public:
    X86Cpu() : Cpu("x86", std::make_unique<X86RegisterFile>(), Endianness::Little) {}
    void reset() override {
        // Reset logic for X86Cpu
    }
};
VERTEX_REGISTER_CPU(X86Cpu)