#pragma once
#include <string>

struct Source {
    std::string path;
    std::string text;
    static bool load(const std::string& path, Source& result);
};