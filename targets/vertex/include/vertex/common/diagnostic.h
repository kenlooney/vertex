#pragma once
#include <string>
#include "span.h"
class Diagnostic {
public:
    virtual ~Diagnostic() = default;
    Diagnostic(std::string filename, std::string message, Span span)
        : filename_(std::move(filename)), message_(std::move(message)), span_(std::move(span)) {}
    virtual void report() = 0;
    Diagnostic(const Diagnostic&) = delete;
    Diagnostic& operator=(const Diagnostic&) = delete;
    std::string filename() const { return filename_; }
    std::string message() const { return message_; }
    Span span() const { return span_; }

   
private:
    std::string filename_;
    std::string message_;
    Span span_;

};