#pragma once
#include <cstdint>
#include <limits>
#include <string>

bool parse_integer(const std::string& text, std::uint64_t& value);
bool parse_byte(const std::string& text, std::uint8_t& value);
bool parse_word(const std::string& text, std::uint16_t& value);
bool parse_dword(const std::string& text, std::uint32_t& value);
bool parse_qword(const std::string& text, std::uint64_t& value);