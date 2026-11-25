#pragma once
#include <cstdint>
#include <limits>
#include <string>

bool parse_integer(const std::string& text, std::uint64_t& value);
bool parse_byte(const std::string& text, std::uint8_t& value);