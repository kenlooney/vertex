# Vertex

**A C++ framework for building extensible assemblers and compilers.**

Vertex is an experimental framework for assembling and compiling through interchangeable CPU, language, and grammar components. Its flagship project, **osasm**, is intended to grow into an assembler with its own assembly language and grammar, starting with a simplified x86 target.

The project is under active development. The plugin loading foundation, KASM lexer,
expression parser/AST, line-to-statement parser, semantic analysis, and `db` byte
collection are in place. CPU plugins declare their default data endianness.
Returning collected bytes, binary-file output, instruction-specific operand
validation, intermediate representations, and instruction encoding remain unfinished.

## What exists today

- Runtime-loaded CPU, language, and grammar plugin interfaces.
- A CPU register file owned by its CPU instance.
- An `Endianness` enum (`Little`/`Big`) and a CPU default data byte-order accessor.
- `Source`, `Token`, and `Span` types, plus shared lexer and parser foundations.
- A shared `kasmlang` library (Ken's Assembly Language) with a `KasmGrammar` that
  creates a `KasmLexer` and `KasmParser`.
- Lexing for labels, identifiers, integers, punctuation, and traditional/C-style
  comments.
- Modern-prefix and legacy-suffix integer literals in binary, octal, decimal, and
  hexadecimal.
- Expression parsing for `+`, `-`, `*`, `/`, `%`, operator precedence, and nested
  parentheses.
- An index-based expression AST arena plus an overflow-checked tree evaluator.
- Newline tokens for LF, CRLF, and CR source lines, including correct handling around
  line comments.
- A shared `Statement` boundary with KASM label, instruction, and directive
  statement types.
- Line-to-statement parsing with comma-delimited operand groups.
- KASM directive recognition and initial semantic validation for `db` byte values.
- Grammar-level statement parsing, semantic analysis, and byte-collection contracts,
  keeping language-specific behavior out of the shared parser and assembler.
- OSASM orchestration of those stages, collecting `db` operands into a byte vector
  in source order without publishing partial bytes on collection failure.
- Width-checked `uint8_t` parsing built on the shared integer conversion utility.
- A shared semantic analyzer interface and KASM semantic analyzer implementation.
- Shared `Option`/`Options` utilities and an `Assembler` framework interface.
- Shared numeric conversion utilities and focused CTest coverage for lexer, parser,
  AST, comments, integer handling, statements, and semantics.
- An `osasm` command-line target with input, output, and CPU-plugin arguments.

The KASM frontend and OSASM assembler are still experimental and are not yet a
complete assembler.

## Byte collection and endianness

The current data-only pipeline is:

```text
source -> KASM lexing -> statements -> semantic checks -> db byte vector
```

Each comma-delimited `db` operand contributes one byte. For example,
[`examples/directives.asm`](examples/directives.asm):

```asm
db 0x5
db 0xFF, 42
db 0x1, 0x2, 0x3, 0x4, 0x5
```

produces these eight byte values:

```text
5, 255, 42, 1, 2, 3, 4, 5
```

The collector currently accepts only `db` directives; labels, instructions, and
other directives are rejected at this stage. Each operand must be a single integer
token representing a value from 0 to 255. Expression evaluation is not yet
connected to directive operands.

Bytes currently remain local to `OsasmAssembler::assemble`; they are not returned,
printed, or written to the requested output file. The assemble result remains
unsuccessful even after valid byte collection, so the CLI still exits with code 1.

CPU plugins specify their default data byte order through the `Cpu` constructor
and expose it through `endianness()`. The x86 plugin declares `Endianness::Little`.
This is metadata for future multi-byte emission, not an instruction encoder.
Endianness does not change `db` output: operands always stay in source order.
Future multi-byte values will need explicit target-endian serialization.

## Project direction

The intended osasm frontend is:

```text
assembly source -> lexer -> parser -> AST -> IR -> CPU-specific output
```

CPU plugins will provide target-specific register and instruction information. The grammar and frontend will define the assembly language and translate it into an intermediate representation before handing off target-specific work.

The next parser milestone is attaching parsed expression trees to statement operand
groups. Instruction-specific semantic validation and IR lowering will follow.

## Build and test

Requirements: CMake 3.24 or newer and a C++17 compiler.

Configure the project:

```sh
cmake -S . -B build
```

Build and run the OSASM semantic test:

```sh
cmake --build build --target osasm_semantic_test
ctest --test-dir build -R osasm_semantic_test --output-on-failure
```

For a multi-configuration generator such as Visual Studio, specify the configuration:

```powershell
cmake --build build --config Debug --target osasm_semantic_test
ctest --test-dir build -C Debug -R osasm_semantic_test --output-on-failure
```

The semantic test loads [`examples/directives.asm`](examples/directives.asm), then
verifies directive recognition, comma-delimited operand grouping, and byte-range
validation. The statement test separately covers labels and instructions, while the
expression tests cover AST construction, evaluation, precedence, grouping, mixed
numeric bases, and arithmetic error cases.

The `osasm_bytes_test` checks exact byte vectors for mixed `db`/`dw` statements
in little- and big-endian order. It also checks that an overflowing `dw` value
is rejected with an error without modifying the caller's output vector.

### Debugging in VS Code on Windows

The checked-in [launch configuration](.vscode/launch.json) uses the Microsoft C++
debugger. With the C/C++ and CMake Tools extensions installed, select the
`windows-debug` configure and build presets, then build `osasm` and `x86_cpu`.
In Run and Debug, select **Debug osasm (directives)** and click the green arrow
or press F5.

The configuration supplies the directive fixture, output path, and CPU-plugin
path. A breakpoint at the final return in `OsasmAssembler::assemble` lets you
inspect the collected `output` vector. Rebuild after code changes before starting
another debug session.

## Automated releases

Pushes to `dev` publish prereleases named `vertex-dev-v<version>`. Pushes or merges to
`main` publish stable releases named `vertex-v<version>`. Both names use the version
declared by `project(... VERSION ...)` in [`CMakeLists.txt`](CMakeLists.txt).

The release workflow runs on GitHub's standard Ubuntu, Windows, and macOS runners.
It produces packages for:

- Windows x86_64 and ARM64
- Linux x86_64 and ARM64
- macOS x86_64 and ARM64

ARM jobs are cross-compiled; no paid larger or architecture-specific runners are
used. Dev snapshots for the same CMake version are replaced with the latest push.
Stable versions are immutable, so bump the CMake project version before merging a
new stable release.

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

For example, a CPU plugin implements its register file, declares its default data
endianness, and implements `reset()`:

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
    MyCpu()
        : Cpu("mycpu", std::make_unique<MyRegisterFile>(), Endianness::Little) {}

    void reset() override {}
};

VERTEX_REGISTER_CPU(MyCpu)
```

Build a plugin with `vertex_add_plugin(<target> <sources...>)`. The example CPU plugin is in [`targets/cpus`](targets/cpus).

The `osasm` executable accepts input, output, and CPU-plugin paths:

```sh
osasm -i <input-file> -o <output-file> <cpu-plugin>
```

Plugins exchange C++ objects with Vertex, so build them with a compatible compiler and runtime. Keep plugin-created objects alive only while their plugin is loaded. Bump a plugin kind's ABI version whenever its base class or plugin interface changes.

The CPU and grammar plugin ABI versions are currently **2**. Rebuild older CPU
plugins for the endianness member and older grammar plugins for the new
grammar-level contracts; plugins declaring the previous ABI versions are rejected.

## Repository layout

```text
targets/
  vertex/       Core types, parser foundations, plugin interfaces, and loaders
  utils/        Shared numeric conversion and utility code
  osasm/        osasm command-line executable
  osasmcore/    osasm assembly orchestration and assembler integration
  cpus/         CPU plugins
  languages/    Language plugins and language frontends
    kasmlang/   KASM lexer, parser, AST, statements, semantics, and db byte collection
tests/          CTest-based tests
examples/       Sample assembly input
```
