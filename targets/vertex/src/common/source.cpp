#include "vertex/common/source.h"
#include <fstream>
#include <sstream>

void Source::load(const std::string& path, Source& result) {
    result.path = path;
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    result.text = buffer.str();
}