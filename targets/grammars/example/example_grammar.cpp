#include "vertex/plugins/grammarplugin.h"

class ExampleGrammar : public Grammar {
public:
    ExampleGrammar() : Grammar("example") {}
};

VERTEX_REGISTER_GRAMMAR(ExampleGrammar)
