# vertex
A framework to help build assemblers and compilers

## Plugins
CPUs, languages and grammars are shared libraries loaded at runtime. Each kind has a base class to subclass, a macro to register it and a loader that loads it:

| Kind | Base class | Header | Register with | Load with |
|------|------------|--------|---------------|-----------|
| CPU | `Cpu` | `vertex/plugins/cpuplugin.h` | `VERTEX_REGISTER_CPU` | `CpuLoader` |
| Language | `Language` | `vertex/plugins/languageplugin.h` | `VERTEX_REGISTER_LANGUAGE` | `LanguageLoader` |
| Grammar | `Grammar` | `vertex/plugins/grammarplugin.h` | `VERTEX_REGISTER_GRAMMAR` | `GrammarLoader` |

To write one, subclass the base class with a default constructor and register it in one source file:

```cpp
#include "vertex/plugins/cpuplugin.h"

class MyCpu : public Cpu {
public:
    MyCpu() : Cpu("mycpu") {}
};

VERTEX_REGISTER_CPU(MyCpu)
```

Build it with `vertex_add_plugin(<target> <sources...>)` (see the examples in [targets/cpus](targets/cpus), [targets/languages](targets/languages) and [targets/grammars](targets/grammars)). Load it with the matching loader; `osasm <path-to-plugin>` loads a CPU plugin. A single library may register several kinds.

Plugins must be built with the same compiler and runtime as vertex. Bump a kind's ABI version (e.g. `VERTEX_CPU_PLUGIN_ABI_VERSION`) whenever its base class changes so that older plugins are rejected.

### Adding a plugin kind
1. Add the base class under `vertex/common`.
2. Add `vertex/plugins/<kind>plugin.h` with an ABI version, `VERTEX_DECLARE_PLUGIN_KIND(Base, id, "name", version)` and a `VERTEX_REGISTER_<KIND>` wrapper around `VERTEX_REGISTER_PLUGIN`.
3. Add `vertex/loaders/<kind>loader.h` with a loader deriving from `PluginLoader<Base>`.

Use `cpuplugin.h` and `cpuloader.h` as templates.
