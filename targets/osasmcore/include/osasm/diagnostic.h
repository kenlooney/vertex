#pragma once
#include "vertex/common/diagnostic.h"
#include <iostream>

class OsasmDiagnostic : public Diagnostic {
public:
    using Diagnostic::Diagnostic;
    void report() override {
        // Custom reporting logic for OsasmDiagnostic
        std::cerr << "Error:" << filename() << ":" << span().begin << ":" << span().end << ": " << message() << std::endl;
    }
};