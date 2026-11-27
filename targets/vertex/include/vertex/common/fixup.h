#pragma once
#include <cstddef>
#include <string>
#include "span.h"

struct Fixup
{
    std::string target;
    std::size_t instruction_offset;
    std::size_t next_offset;
    Span source_span;
    std::string encoder_key = "jmp";
};
