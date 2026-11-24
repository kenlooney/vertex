# Vertex

**A C++ framework for building extensible assemblers and compilers.**

Vertex is an experimental framework for assembling and compiling through interchangeable CPU, language, and grammar components. Its flagship project, **osasm**, is intended to grow into an assembler with its own assembly language and grammar, starting with a simplified x86 target.

The project is under active development. The plugin loading foundation and the first source-to-token path are in place; parsing, intermediate representations, and code generation are future work.

## What exists today

- Runtime-loaded CPU, language, and grammar plugin interfaces.
- A CPU register file owned by its CPU instance.
- `Source`, `Token`, and `Span` types, plus a lexer base class.
- An `OsasmGrammar` that creates an `OsasmLexer` for a source file.
- A small lexer prototype and a CTest that exercises source loading through tokenization.
- An `osasm` command-line target that currently demonstrates loading a CPU plugin.

The lexer and grammar are early prototypes, not a complete assembler frontend.

## Project direction

The intended osasm frontend is:

```text
assembly source -> lexer -> parser -> AST -> IR -> CPU-specific output
```

CPU plugins will provide target-specific register and instruction information. The grammar and frontend will define the assembly language and translate it into an intermediate representation before handing off target-specific work.

## Build and test

Requirements: CMake 3.24 or newer and a C++17 compiler.

Configure the project:

```sh
cmake -S . -B build
```

Build and run the osasm grammar test:

```sh
cmake --build build --target osasm_grammar_test
ctest --test-dir build -R osasm_grammar_test --output-on-failure
```

For a multi-configuration generator such as Visual Studio, specify the configuration:

```powershell
cmake --build build --config Debug --target osasm_grammar_test
ctest --test-dir build -C Debug -R osasm_grammar_test --output-on-failure
```

The test currently uses `examples/test.asm` and checks the initial source-to-token path.

## Automated releases

Pushes to `dev` publish prereleases named `vertex-dev-v<version>`. Pushes or merges to
`main` publish stable releases named `vertex-v<version>`. Both names use the version
declared by `project(... VERSION ...)` in [`CMakeLists.txt`](CMakeLists.txt).

The release workflow runs on GitHub's standard Ubuntu, Windows, and macOS runners.
It produces packages for:

- Windows x86_64 and ARM64
- Linux x86_64, ARM64, and RISC-V 64
- macOS x86_64 and ARM64

ARM and RISC-V jobs are cross-compiled; no paid larger or architecture-specific
runners are used. Dev snapshots for the same CMake version are replaced with the
latest push. Stable versions are immutable, so bump the CMake project version before
merging a new stable release.

Retention keeps the newest four dev prereleases and four stable releases. Their
eight release tags are retained along with up to twelve older
`vertex-dev-v*`/`vertex-v*` tags, for a maximum of twenty managed tags. Unrelated
repository tags are not modified.

## Runtime plugins

CPU, language, and grammar plugins are shared libraries. Each plugin kind has a base class, a registration macro, and a loader:

| Kind | Base class | Registration macro | Loader |
| --- | --- | --- | --- |
| CPU | `Cpu` | `VERTEX_REGISTER_CPU` | `CpuLoader` |
| Language | `Language` | `VERTEX_REGISTER_LANGUAGE` | `LanguageLoader` |
| Grammar | `Grammar` | `VERTEX_REGISTER_GRAMMAR` | `GrammarLoader` |

For example, a CPU plugin implements its register file and passes it to the `Cpu` base class:

```cpp
#include <memory>
#include "vertex/common/registers.h"
#include "vertex/plugins/cpuplugin.h"

class MyRegisterFile : public RegisterFile {
public:
    MyRegisterFile() {
        addRegister(std::make_unique<Register>("a"));
    }
};

class MyCpu : public Cpu {
public:
    MyCpu() : Cpu("mycpu", std::make_unique<MyRegisterFile>()) {}
};

VERTEX_REGISTER_CPU(MyCpu)
```

Build a plugin with `vertex_add_plugin(<target> <sources...>)`. The example CPU plugin is in [`targets/cpus`](targets/cpus).

The `osasm` executable currently accepts a CPU plugin path:

```sh
osasm <cpu-plugin>
```

Plugins exchange C++ objects with Vertex, so build them with a compatible compiler and runtime. Keep plugin-created objects alive only while their plugin is loaded. Bump a plugin kind's ABI version whenever its base class or plugin interface changes.

## Repository layout

```text
targets/
  vertex/       Core types, plugin interfaces, and loaders
  osasm/        osasm command-line executable
  osasmcore/    osasm grammar and lexer
  cpus/         CPU plugins
  languages/    Language plugins
  grammars/     Grammar plugins
tests/          CTest-based tests
examples/       Sample assembly input
```
