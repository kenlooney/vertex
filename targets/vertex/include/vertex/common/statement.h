#pragma once
#include "span.h"
#include <string_view>
#include <string>
class Statement
{
public:
    explicit Statement(Span span, std::string kind) : span_(span), kind_(std::move(kind)) {}
    virtual ~Statement() = default; 
    Span span() const { return span_; }
    virtual std::string_view kind() const { return kind_; }
private:
    Span span_;
    std::string kind_;
};