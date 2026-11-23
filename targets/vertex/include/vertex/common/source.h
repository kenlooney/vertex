#pragma once
#include <string>

struct Source {
    std::string path;
    std::string text;
    static void load(const std::string& path, Source& result);
};