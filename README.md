# Vertex

**A C++ framework for building extensible assemblers and compilers.**

Vertex is an experimental framework for assembling and compiling through interchangeable CPU, language, and grammar components. Its flagship project, **osasm**, is intended to grow into an assembler with its own assembly language and grammar, starting with a simplified x86 target.

The project is under active development. The plugin loading foundation, KASM lexer,
expression parser/AST, line-to-statement parser, semantic analysis, endian-aware
`db`/`dw`/`dd`/`dq` data collection, basic x86 instruction encoding, and raw
binary file output are in place. CPU plugins declare their default data endianness.
Returning collected bytes and a broader intermediate representation remain future work.

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
- KASM directive recognition and unsigned range validation for `db`, `dw`, `dd`,
  and `dq`, plus optional `format` metadata.
- Grammar-level statement parsing, semantic analysis, and byte-collection contracts,
  keeping language-specific behavior out of the shared parser and assembler.
- OSASM orchestration of those stages, collecting data operands into a byte vector
  in source order and the CPU's data byte order without publishing partial bytes
  on collection failure.
- Built-in raw binary output, with open/write/close failure diagnostics and a
  successful assemble result only after the output file is closed successfully.
- Width-checked byte, word, dword, and qword parsing built on the shared integer
  conversion utility.
- A shared semantic analyzer interface and KASM semantic analyzer implementation.
- Shared `Option`/`Options` utilities and an `Assembler` framework interface.
- Shared numeric conversion utilities and focused CTest coverage for lexer, parser,
  AST, comments, integer handling, statements, and semantics.
- An `osasm` command-line target with input, output, and CPU-plugin arguments.

The KASM frontend and OSASM assembler are still experimental and are not yet a
complete assembler.

## Byte collection and endianness

The byte collection pipeline is:

```text
source -> KASM lexing -> statements -> semantic checks -> data and instruction bytes -> raw binary file
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

The collector accepts `db`, `dw`, `dd`, and `dq` for unsigned 1-, 2-, 4-, and 8-byte
values, plus optional `format` metadata that contributes no bytes. Each data operand
must be a single integer token within its directive's unsigned range. Expression
evaluation is not yet connected to directive operands.

With the x86 plugin, `mov` and `add` accept `al`, `cl`, `dl`, `bl`, `ah`, `ch`,
`dh`, or `bh` and an immediate expression in the range `0`–`255`. `hlt` takes zero operands. For
example, `mov al, 6 * 7`, `add al, 5`, and `hlt` emit `B0 2A 80 C0 05 F4` in
source order. Data directives can appear between instructions. A bare operand
such as `ah` names the register if the CPU defines it. Spell a hexadecimal
immediate such as `0Ah` or `0xA` to avoid that ambiguity.

KASM converts each instruction's operands to register names or evaluated
immediates. `X86Encoder::encode` looks up the mnemonic in its table and calls a
helper that validates the operand form and writes its bytes. Labels and further
instruction forms remain future work.

`OsasmAssembler::assemble` writes the collected bytes to the specified output file
in binary mode, replacing any existing contents. Validation and collection finish
before the file is opened. File open, write, or close failures are reported as
errors; the CLI exits with code 0 only after successful assembly and plugin cleanup.
The byte vector is not returned in the assemble result. A failure during writing
may leave a partial file; output replacement is not atomic.

CPU plugins specify their default data byte order through the `Cpu` constructor
and expose it through `endianness()`. The x86 plugin declares `Endianness::Little`.
The collector uses this setting for multi-byte data emission.
Endianness does not change `db` output: operands always stay in source order.
`dw`, `dd`, and `dq` serialize each value in the selected CPU's data byte order.

## Project direction

The intended osasm frontend is:

```text
assembly source -> lexer -> parser -> AST -> IR -> CPU-specific output
```

CPU plugins will provide target-specific register and instruction information. The grammar and frontend will define the assembly language and translate it into an intermediate representation before handing off target-specific work.

The next parser milestone is attaching parsed expression trees to statement operand
groups. The current instruction path parses and evaluates immediate expressions
during byte collection.

### Output formats

Raw binary is the built-in output target: collected bytes are written directly
to a file, without executable headers or container metadata. No external format
plugin is needed.

The assembler defaults to `raw` when no format is specified and accepts
`format raw` in source. Unsupported formats are rejected before opening the
output file. Its options API also accepts a `format` setting; an empty setting
or a conflict with the source request is rejected. The CLI does not yet expose
a format argument.

An output-format plugin system is a longer-term design goal, separate from CPU
and grammar plugins:

```text
source -> grammar -> validation -> encoding -> output-format plugin -> file
```

The grammar will expose an optional source format request. The assembler will
resolve source, command-line, and default selection, then hand the prepared output
to the selected format plugin for final layout and serialization. The plugin
will report success or an error; overall assembly will succeed only after output
is written successfully. Raw binary is the intended default when no format is
requested.

Raw binary needs only the emitted bytes. Future structured formats such as ELF
and PE will also need sections, symbols, relocations, entry-point information,
and target metadata. Their shared output model will be developed when those
formats are introduced; format-specific loading and writing will not live in KASM.

The `format testformat` collector test exercises only a placeholder directive and
debug message; the assembler rejects that name for actual file output. No
output-format plugins or structured-format writers exist yet.

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

The `osasm_bytes_test` checks exact byte vectors for `db`, `dw`, `dd`, and `dq`
in little- and big-endian order, including zero and maximum `dd`/`dq` values.
It also checks that overflowing values, missing `dd`/`dq` operands, and unsupported
`dd`/`dq` expressions are rejected with an error without modifying the caller's
output vector.

A placeholder `format testformat` fixture checks the format debug message and
verifies that the directive adds no bytes in either endian mode. This does not
implement or validate a real output-file format.

The `osasm_output_test` verifies exact raw file contents, default and explicit
raw selection, output truncation, unsupported/conflicting formats, invalid input,
and output-open failures. Error diagnostics from these deliberate failure cases
are expected in a passing test run. The test creates its output in a temporary
directory and removes that directory during cleanup; it does not leave a binary
file to inspect.

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

### Inspecting raw output in VS Code

Let **Debug osasm (directives)** run to completion. It writes
[`bin/Debug/output.bin`](bin/Debug/output.bin), which remains available after the
debug session. In the Explorer, right-click the file and choose **Open With...**
then **Hex Editor** (install the Microsoft Hex Editor extension if needed).

For [`examples/directives.asm`](examples/directives.asm), the file contains exactly
these eight bytes, with no headers:

```text
05 FF 2A 01 02 03 04 05
```

Running the same configuration again replaces the file's contents.

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

The CPU and grammar plugin ABI versions are both **4**. Rebuild older CPU plugins
for the encoder interface and older grammar plugins for CPU-aware byte collection.
Plugins declaring previous ABI versions are rejected.

## Repository layout

```text
targets/
  vertex/       Core types, parser foundations, plugin interfaces, and loaders
  utils/        Shared numeric conversion and utility code
  osasm/        osasm command-line executable
  osasmcore/    osasm assembly orchestration and assembler integration
  cpus/         CPU plugins
  languages/    Language plugins and language frontends
    kasmlang/   KASM lexer, parser, AST, statements, semantics, and data byte collection
tests/          CTest-based tests
examples/       Sample assembly input
```
