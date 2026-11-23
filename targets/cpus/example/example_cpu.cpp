#include "vertex/plugins/cpuplugin.h"

class ExampleCpu : public Cpu {
public:
    ExampleCpu() : Cpu("example") {}
};

VERTEX_REGISTER_CPU(ExampleCpu)
