#include "vertex/common/source.h"
#include <fstream>
#include <sstream>

bool Source::load(const std::string& path, Source& result) {
    if (path.empty()) {
        return false;
    }
    result.path = path;
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    result.text = buffer.str();
    return !result.text.empty();
}