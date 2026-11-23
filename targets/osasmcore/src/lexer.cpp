#include "osasm/lexer.h"
#include "vertex/common/span.h"
std::vector<Token> OsasmLexer::tokenize() {
    // Tokenization logic for OsasmLexer goes here
    std::size_t cursor = 0;
    while(this->source().text.size() > cursor) {
        std::size_t begin = cursor;
       
        // Skip whitespace
        while(cursor < this->source().text.size() && std::isspace(this->source().text[cursor])) {
            ++cursor;
        }

        // Identify identifiers
        if(cursor < this->source().text.size() && std::isalpha(this->source().text[cursor])) {
            while(cursor < this->source().text.size() && (std::isalnum(this->source().text[cursor]) || this->source().text[cursor] == '_')) {
                ++cursor;
            }
            add_token("identifier", this->source().text.substr(begin, cursor - begin), Span::set(begin, cursor));
            continue;
        }

        // Identify numbers
        if(cursor < this->source().text.size() && std::isdigit(this->source().text[cursor])) {
            while(cursor < this->source().text.size() && std::isdigit(this->source().text[cursor])) {
                ++cursor;
            }
            add_token("number", this->source().text.substr(begin, cursor - begin), Span::set(begin, cursor));
            continue;
        }


       ++cursor;
    }
    return get_tokens();
}