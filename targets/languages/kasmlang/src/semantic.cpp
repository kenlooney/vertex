#include "kasmlang/semantic.h"
#include "kasmlang/statements/statements.h"
#include "utils/integer.h"

bool KasmSemanticAnalyzer::analyze(
    const std::vector<std::unique_ptr<Statement>>& statements,
    std::string& error)
{
    error.clear();

    for (const auto& stmt : statements) {
        if(const auto* dirStmt =
            dynamic_cast<const DirectiveStatement*>(stmt.get())) {
            if(dirStmt->name() == "db") {
                if(dirStmt->operands().empty()) {
                    error = "The 'db' directive requires at least one operand.";
                    return false;
                }

                for(const auto& operand : dirStmt->operands()) {
                    if(operand.size() != 1 || operand.front().kind != "integer") {
                        error = "The 'db' directive expects integer byte operands.";
                        return false;
                    }

                    std::uint8_t value;
                    if(!parse_byte(operand.front().text, value)) {
                        error = "The 'db' directive contains an invalid byte value: "
                            + operand.front().text;
                        return false;
                    }
                }
            }
            if(dirStmt->name() == "dw") {
                if(dirStmt->operands().empty()) {
                    error = "The 'dw' directive requires at least one operand.";
                    return false;
                }

                for(const auto& operand : dirStmt->operands()) {
                    if(operand.size() != 1 || operand.front().kind != "integer") {
                        error = "The 'dw' directive expects integer word operands.";
                        return false;
                    }

                    std::uint16_t value;
                    if(!parse_word(operand.front().text, value)) {
                        error = "The 'dw' directive contains an invalid word value: "
                            + operand.front().text;
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
