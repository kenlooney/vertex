#pragma once
#include <string>
#include <vector>

enum class SymbolType
{
    Label
};

struct Symbol
{
    SymbolType type;
    std::string name;
    std::size_t offset;
};

class SymbolTable
{
public:
    bool addSymbol(const Symbol &symbol)
    {
        if (hasSymbol(symbol.name))
            return false;

        symbols.push_back(symbol);
        return true;
    }
    bool hasSymbol(const std::string &name) const
    {
       return getSymbol(name) != nullptr;
    }

    const std::vector<Symbol> &getSymbols() const
    {
        return symbols;
    }
    const Symbol *getSymbol(const std::string &name) const
    {
        for (const auto &symbol : symbols)
        {
            if (symbol.name == name)
                return &symbol;
        }
        return nullptr;
    }

private:
    std::vector<Symbol> symbols;
};