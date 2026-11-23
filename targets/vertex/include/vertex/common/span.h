#pragma once
#include <cstddef>
struct Span {
    std::size_t begin;
    std::size_t end;
    static Span set(std::size_t begin, std::size_t end) {
        Span span;
        span.begin = begin;
        span.end = end;
        return span;
    }
};