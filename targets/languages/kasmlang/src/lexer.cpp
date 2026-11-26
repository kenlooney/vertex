#include "kasmlang/lexer.h"
#include "vertex/common/span.h"

#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>

constexpr std::string_view directives[] = {
    "db","dw","dd","dq",
    "format"
};
namespace
{
    char lower_ascii(char ch)
    {
        return ch >= 'A' && ch <= 'Z' ? static_cast<char>(ch - 'A' + 'a') : ch;
    }

    bool binary_digit(char ch)
    {
        return ch == '0' || ch == '1';
    }

    bool octal_digit(char ch)
    {
        return ch >= '0' && ch <= '7';
    }

    bool decimal_digit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    bool hexadecimal_digit(char ch)
    {
        return decimal_digit(ch) || (lower_ascii(ch) >= 'a' && lower_ascii(ch) <= 'f');
    }

    bool identifier_character(char ch)
    {
        const char lower = lower_ascii(ch);
        return decimal_digit(ch) || (lower >= 'a' && lower <= 'z') || ch == '_';
    }

    bool all_digits(const std::string &text, std::size_t begin, std::size_t end, bool (*is_digit)(char))
    {
        if (begin == end)
        {
            return false;
        }

        for (std::size_t cursor = begin; cursor < end; ++cursor)
        {
            if (!is_digit(text[cursor]))
            {
                return false;
            }
        }
        return true;
    }

    std::size_t number_end(const std::string &text, std::size_t begin)
    {
        if (text[begin] == '0' && begin + 1 < text.size())
        {
            const char prefix = lower_ascii(text[begin + 1]);
            bool (*is_digit)(char) = nullptr;

            if (prefix == 'b')
            {
                is_digit = binary_digit;
            }
            else if (prefix == 'o')
            {
                is_digit = octal_digit;
            }
            else if (prefix == 'd')
            {
                is_digit = decimal_digit;
            }
            else if (prefix == 'x')
            {
                is_digit = hexadecimal_digit;
            }

            if (is_digit != nullptr)
            {
                std::size_t end = begin + 2;
                while (end < text.size() && is_digit(text[end]))
                {
                    ++end;
                }

                if (end == begin + 2 || (end < text.size() && identifier_character(text[end])))
                {
                    return begin;
                }
                return end;
            }
        }

        std::size_t end = begin;
        while (end < text.size() && identifier_character(text[end]))
        {
            ++end;
        }

        if (decimal_digit(text[begin]))
        {
            const char suffix = lower_ascii(text[end - 1]);
            if (suffix == 'b')
            {
                return all_digits(text, begin, end - 1, binary_digit) ? end : begin;
            }
            if (suffix == 'o' || suffix == 'q')
            {
                return all_digits(text, begin, end - 1, octal_digit) ? end : begin;
            }
            if (suffix == 'd')
            {
                return all_digits(text, begin, end - 1, decimal_digit) ? end : begin;
            }
            if (suffix == 'h')
            {
                return all_digits(text, begin, end - 1, hexadecimal_digit) ? end : begin;
            }
            return all_digits(text, begin, end, decimal_digit) ? end : begin;
        }

        if (end > begin + 1 && lower_ascii(text[end - 1]) == 'h' && all_digits(text, begin, end - 1, hexadecimal_digit))
        {
            return end;
        }

        return begin;
    }
} // namespace kasmlang

std::vector<Token> KasmLexer::tokenize()
{
    std::string text = this->source().text;
    std::size_t cursor = 0;
    while (text.size() > cursor)
    {
        char ch = text[cursor]; // look-ahead character

        if (ch == ' ' || ch == '\t')
        {
            ++cursor;
            continue;
        }
        std::size_t begin = cursor;

        if (ch == '\r' || ch == '\n')
        {
            ++cursor;
            if (ch == '\r' && cursor < text.size() && text[cursor] == '\n')
            {
                ++cursor;
            }
            add_token("newline", "\\n", Span::set(begin, cursor));
            continue;
        }

        // Comment: starts with a semicolon and goes until the end of the line
        if (ch == ';')
        {
            while (cursor < text.size()
                   && text[cursor] != '\r'
                   && text[cursor] != '\n')
            {
                ++cursor;
            }
            continue;
        }
        // Modern Single-line comment
        if (ch == '/' && cursor + 1 < text.size() && text[cursor + 1] == '/')
        {
            cursor += 2;
            while (cursor < text.size()
                   && text[cursor] != '\r'
                   && text[cursor] != '\n')
            {
                ++cursor;
            }
            continue;
        }
        // Modern block comment (/* ... */)
        if (ch == '/' && cursor + 1 < text.size() && text[cursor + 1] == '*')
        {
            cursor += 2;
            while (cursor + 1 < text.size() && !(text[cursor] == '*' && text[cursor + 1] == '/'))
            {
                ++cursor;
            }
            if (cursor + 1 < text.size())
            {
                cursor += 2;
            }
            continue;
        }
        // Label: starts with a letter and ends with a colon
        if (letter(ch))
        {
            while (cursor < text.size() && (letter(text[cursor]) || digit(text[cursor]) || text[cursor] == '_'))
            {
                ++cursor;
            }
            if (cursor < text.size() && text[cursor] == ':')
            {
                ++cursor;
                std::string label = text.substr(begin, cursor - begin - 1);
                add_token("label", label, Span::set(begin, cursor));
                continue;
            }
            cursor = begin;
        }

        cursor = number_end(text, begin);
        if (cursor > begin)
        {
            add_token("integer", text.substr(begin, cursor - begin), Span::set(begin, cursor));
            continue;
        }

        // Identifier: starts with a letter, followed by letters, digits, or underscores
        if (letter(ch))
        {
            while (cursor < text.size() && (letter(text[cursor]) || digit(text[cursor]) || text[cursor] == '_'))
            {
                ++cursor;
            }
            // Check if the identifier is a directive
            const std::string identifier = text.substr(begin, cursor - begin);
            if (std::find(
                    std::begin(directives),
                    std::end(directives),
                    identifier) != std::end(directives)) {
                add_token("directive", identifier, Span::set(begin, cursor));
                continue;
            }
            add_token("identifier", identifier, Span::set(begin, cursor));
            continue;
        }

        ++cursor;
        add_token("punctuation", text.substr(begin, 1), Span::set(begin, cursor));
    }
    return get_tokens();
}
