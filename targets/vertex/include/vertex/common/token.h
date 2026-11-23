#pragma once
#include <string>
#include "span.h"

struct Token {
    std::string kind;
    std::string text;
    Span span;
};