#include "vertex/plugins/languageplugin.h"

class ExampleLanguage : public Language {
public:
    ExampleLanguage() : Language("example") {}
};

VERTEX_REGISTER_LANGUAGE(ExampleLanguage)
