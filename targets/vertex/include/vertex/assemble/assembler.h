#pragma once
#include <memory>


#include "vertex/common/source.h"
#include "utils/options.h"
#include "vertex/common/cpu.h"
#include "vertex/common/grammar.h"

class Assembler {
public:
    Assembler() = default;
    virtual ~Assembler() = default;
    virtual std::unique_ptr<OptionResult> assemble(
        const Source& source,
        const Options& options) = 0;

    void setCpu(Cpu* cpu) {
        cpu_ = cpu;
    }

    void setGrammar(Grammar* grammar) {
        grammar_ = grammar;
    }

protected:
    Cpu* cpu() const { return cpu_; }
    Grammar* grammar() const { return grammar_; }

private:
    Cpu* cpu_ = nullptr;
    Grammar* grammar_ = nullptr;
};