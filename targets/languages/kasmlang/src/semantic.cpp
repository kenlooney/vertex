#include "kasmlang/semantic.h"
#include "kasmlang/statements/statements.h"
#include "utils/integer.h"

bool KasmSemanticAnalyzer::analyze(
    const std::vector<std::unique_ptr<Statement>> &statements,
    std::string &error)
{
    error.clear();
    format_.clear();
    bool has_format_been_set = false;

    for (const auto &stmt : statements)
    {
        if (const auto *dirStmt =
                dynamic_cast<const DirectiveStatement *>(stmt.get()))
        {
            if (dirStmt->name() == "db")
            {
                if (dirStmt->operands().empty())
                {
                    error = "The 'db' directive requires at least one operand.";
                    return false;
                }

                for (const auto &operand : dirStmt->operands())
                {
                    if (operand.size() != 1 || operand.front().kind != "integer")
                    {
                        error = "The 'db' directive expects integer byte operands.";
                        return false;
                    }

                    std::uint8_t value;
                    if (!parse_byte(operand.front().text, value))
                    {
                        error = "The 'db' directive contains an invalid byte value: " + operand.front().text;
                        return false;
                    }
                }
            }
            if (dirStmt->name() == "dw")
            {
                if (dirStmt->operands().empty())
                {
                    error = "The 'dw' directive requires at least one operand.";
                    return false;
                }

                for (const auto &operand : dirStmt->operands())
                {
                    if (operand.size() != 1 || operand.front().kind != "integer")
                    {
                        error = "The 'dw' directive expects integer word operands.";
                        return false;
                    }

                    std::uint16_t value;
                    if (!parse_word(operand.front().text, value))
                    {
                        error = "The 'dw' directive contains an invalid word value: " + operand.front().text;
                        return false;
                    }
                }
            }
            if (dirStmt->name() == "dd")
            {
                if (dirStmt->operands().empty())
                {
                    error = "The 'dd' directive requires at least one operand.";
                    return false;
                }

                for (const auto &operand : dirStmt->operands())
                {
                    if (operand.size() != 1 || operand.front().kind != "integer")
                    {
                        error = "The 'dd' directive expects integer double word operands.";
                        return false;
                    }

                    std::uint32_t value;
                    if (!parse_dword(operand.front().text, value))
                    {
                        error = "The 'dd' directive contains an invalid double word value: " + operand.front().text;
                        return false;
                    }
                }
            }
            if (dirStmt->name() == "dq")
            {
                if (dirStmt->operands().empty())
                {
                    error = "The 'dq' directive requires at least one operand.";
                    return false;
                }

                for (const auto &operand : dirStmt->operands())
                {
                    if (operand.size() != 1 || operand.front().kind != "integer")
                    {
                        error = "The 'dq' directive expects integer quad word operands.";
                        return false;
                    }

                    std::uint64_t value;
                    if (!parse_qword(operand.front().text, value))
                    {
                        error = "The 'dq' directive contains an invalid quad word value: " + operand.front().text;
                        return false;
                    }
                }
            }
            if (dirStmt->name() == "format")
            {

                if (has_format_been_set)
                {
                    error = "The 'format' directive can only be set once.";
                    return false;
                }
                if (dirStmt->operands().empty())
                {
                    error = "The 'format' directive requires at least one operand.";
                    return false;
                }
                has_format_been_set = true;

                const auto &operands = dirStmt->operands();
                if (operands.size() != 1 ||
                    operands.front().size() != 1 ||
                    operands.front().front().kind != "identifier")
                {
                    error = "The 'format' directive expects one format identifier.";
                    return false;
                }

                std::string format = operands.front().front().text;
                this->format_ = format;
            }
            if (dirStmt->name() == "repeat")
            {
                if (dirStmt->operands().empty())
                {
                    error = "The 'repeat' directive requires at least one operand.";
                    return false;
                }
                // The first operand of the 'repeat' directive should be the repeat count.
                if (dirStmt->operands().front().size() != 3)
                {
                    error = "The 'repeat' directive expects exactly three operands for the repeat count.";
                    return false;
                }
                Token tokenCount  = dirStmt->operands().front()[0];
                if (tokenCount.kind != "integer")
                {
                    error = "The 'repeat' directive expects an integer operand.";
                    return false;
                }
                std::uint64_t repeat_count;
                if(!parse_integer(tokenCount.text, repeat_count))
                {
                    error = "The 'repeat' directive contains an invalid integer value: " + tokenCount.text;
                    return false;
                }
                 Token tokenDirective  = dirStmt->operands().front()[1];
                 if (tokenDirective.kind != "directive")
                 {
                     error = "The 'repeat' directive expects a directive as the second operand.";
                     return false;
                 }
                 if(tokenDirective.text == "db" || tokenDirective.text == "dw" || tokenDirective.text == "dd" || tokenDirective.text == "dq")
                 {
                     // Valid directive for repeat

                 }
                 else
                 {
                     error = "The 'repeat' directive expects a valid data directive as the second operand.";
                     return false;
                 }
                

                
            }
        }
    }

    return true;
}
