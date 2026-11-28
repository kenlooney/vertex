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

std::pair<std::size_t, std::size_t> Source::line_column(std::size_t offset) const
{
    std::size_t line = 1;
    std::size_t column = 1;

    for (std::size_t i = 0; i < offset && i < text.size(); ++i)
    {
        if (text[i] == '\r' || text[i] == '\n')
        {
            if (text[i] == '\n' && i > 0 && text[i - 1] == '\r')
                continue; // CRLF is one line break.

            ++line;
            column = 1;
        }
        else
        {
            ++column;
        }
    }

    return {line, column};
}