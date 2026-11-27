# Vertex

**A C++ framework for building extensible assemblers and compilers.**

Vertex is an experimental framework with interchangeable CPU, language, and
grammar components. Its assembler, **osasm**, translates KASM (Ken's Assembly
Language) to raw binary using a runtime-loaded CPU plugin. The x86 plugin can
encode and decode short, 16-bit near, and 16-bit far jumps.

## What exists today

- Runtime-loaded CPU, language, and grammar plugin interfaces.
- A CPU-owned register file, default data byte order, and optional encoding and
  decoding capabilities.
- A `kasmlang` lexer, parser, and semantic analyzer for labels, instructions,
  directives, and comma-delimited operands. The lexer handles traditional and
  C-style comments and LF, CRLF, and CR line endings.
- Width-checked binary, octal, decimal, and hexadecimal integer literals with
  modern prefixes and legacy suffixes. An overflow-checked expression evaluator
  supports `+`, `-`, `*`, `/`, `%`, precedence, and parentheses.
- Endian-aware `db`, `dw`, `dd`, and `dq` output, optional `format` metadata, and
  symbol lookup for forward and backward jumps.
- Raw binary output through the `osasm` CLI, with validation before the output
  file is opened.
- CTest coverage for plugin loading, KASM parsing and semantics, raw output,
  x86 encoding, and decoding round trips.

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
`dh`, or `bh` and an immediate expression in the range `0`–`255`. Both `hlt`
and `nop` take zero operands. For example, `mov al, 6 * 7`, `add al, 5`, and
`hlt` emit `B0 2A 80 C0 05 F4` in source order. Data directives can appear
between instructions. A bare operand such as `ah` names the register if the
CPU defines it. Spell a hexadecimal immediate such as `0Ah` or `0xA` to avoid
that ambiguity.

KASM passes register names, evaluated immediates, resolved displacements, or
far addresses to the CPU encoder. The x86 encoder looks up the instruction
form in its table and calls a helper that writes the bytes.

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

## x86 jumps and fixups

A label records the current output byte offset and emits zero bytes. It can
appear on its own line or before a statement. The near and far encodings below
assume 16-bit x86 code.

| KASM syntax | Bytes | Target |
| --- | --- | --- |
| `jmp label` | `EB` + one byte | Signed displacement from the end of the jump, `-128` to `127` |
| `jmp near label` | `E9` + two bytes | Signed displacement from the end of the jump, `-32768` to `32767` |
| `jmp far segment:offset` | `EA` + two offset bytes + two segment bytes | Explicit address with two unsigned 16-bit fields |

For a far jump, KASM parses both numeric fields with `parse_word()` and passes
a `FarOperand` directly to the encoder. Each field is written low byte first.
For example, `jmp far 0x1234:0x5678` emits `EA 78 56 34 12` and decodes as
`jmp far 4660:22136`.

Short and near jumps use the same label-fixup process.

1. `collect_jump()` selects the encoder key `jmp` or `jmp near` and passes
   `RelativeOperand{0}`. The x86 plugin emits `EB 00` or `E9 00 00`. The
   collector saves the start offset, end offset, and selected key in a `Fixup`.
2. After collecting all statements, `resolve_jump_fixups()` looks up each
   target in the symbol table and computes `target.offset - fixup.next_offset`.
   The resolver calculates the magnitude before applying the sign so a backward
   jump cannot underflow the unsigned offsets.
3. The resolver passes the signed displacement to the same encoder helper. The
   helper checks the form's range, and the collector checks that the result is
   the same length as the placeholder before copying it into the buffer.
   Malformed jumps, duplicate or unknown labels, and out-of-range targets fail
   before `osasm` opens the output file.

The KASM collector owns the symbol table and fixup list for the current
assembly. The x86 encoder helper owns the opcode and displacement range.
[KASM collector](targets/languages/kasmlang/include/kasmlang/grammar.h) ·
[x86 encoder](targets/cpus/x86/x86_cpu.cpp)

| `Fixup` field | Meaning |
| --- | --- |
| `target` | Label name to look up after layout |
| `instruction_offset` | First byte of the placeholder and where to write the final encoding |
| `next_offset` | First byte after the instruction and the base for the displacement |
| `source_span` | Source range of the jump statement, available for diagnostics |
| `encoder_key` | Encoder table entry for the selected form. Defaults to `jmp` |

This program emits `E9 01 00 90 EA 78 56 34 12 EB F5`.

```asm
start:
    jmp near handoff
    nop
handoff:
    jmp far 0x1234:0x5678
    jmp start
```

The near jump ends at byte `3` and targets byte `4`, so its displacement is
`+1`. The short jump ends at byte `11` and targets byte `0`, so its displacement
is `-11`, encoded as `F5`. The far jump carries its segment and offset in the
instruction, so it uses no label lookup or fixup.

### Adding another jump form

- Use the same encoding form for the placeholder and the final jump. For a
  fixed-size relative form, save the encoder key in the fixup and let the
  encoder determine the length and check the displacement range.
  [Fixup definition](targets/vertex/include/vertex/common/fixup.h)
- If the assembler will choose between short and near jumps, allow for layout
  changes before patching. Growing a jump shifts later labels and fixups, so
  recompute offsets and displacements until the instruction sizes are settled.
- Extend the encoding test with exact forward and backward bytes, both
  displacement limits, and a case just beyond each limit. The existing test
  also checks that failed assembly preserves an earlier output file.
  [x86 encoding test](tests/osasm_x86_encoding_test.cmake)

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

You’ll need CMake 3.24 or newer and a C++17 compiler.

Configure, build, and run the test suite with a single-configuration generator.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

For a multi-configuration generator such as Visual Studio, use the same
configuration for the build and the tests.

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
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

The `osasm_x86_encoding_test` checks the short and near jump encodings at both
displacement limits, among other x86 instruction and failure cases. The
far-jump example above gives the expected bytes for an explicit address.

### x86 decoding round trip

The `osasm_x86_decode_test` assembles [examples/decode.asm](examples/decode.asm)
and verifies the raw bytes `B0 01 B1 02 90 E9 01 00 90 EB FA E9 F7 FF`. The
loaded x86 plugin decodes them to this text.

```text
mov al, 1
mov cl, 2
nop
jmp near relative 1
nop
jmp relative -6
jmp near relative -9
```

To build the test and see its output on Windows, use the `windows-debug` build
directory after configuring the project.

```powershell
cmake --build build/windows-debug --config Debug --target osasm_x86_decode_test
ctest --test-dir build/windows-debug -C Debug -V -R '^osasm_x86_decode_test$'
```

The x86 decoder recognizes `90` (`nop`), `B0`–`B7` (`mov reg8, imm8`), `EB`
(short jump), `E9` (16-bit near jump), and `EA` (16-bit far jump). Short and
near jumps print a signed displacement. Far jumps print `segment:offset` in
decimal. Each successful call writes one instruction and advances the byte
offset. Unknown opcodes, truncated instructions, and end of input return
`false` and preserve the offset and text. Disassembly is currently available
through the decoder API and the round-trip test.

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

CPU, language, and grammar plugins are shared libraries. Each kind has a base
class, a registration macro, and a loader.

| Kind | Base class | Registration macro | Loader | ABI |
| --- | --- | --- | --- | ---: |
| CPU | `Cpu` | `VERTEX_REGISTER_CPU` | `CpuLoader` | 7 |
| Language | `Language` | `VERTEX_REGISTER_LANGUAGE` | `LanguageLoader` | 1 |
| Grammar | `Grammar` | `VERTEX_REGISTER_GRAMMAR` | `GrammarLoader` | 4 |

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
        : Cpu("mycpu", std::make_unique<MyRegisterFile>(), Endianness::Little, nullptr) {}

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

`Cpu::decoder()` returns the CPU's optional decoder. The x86 plugin creates its
decoder on first access. Call `Decoder::decode()` with the CPU's register file,
the input bytes, a byte offset, and a string for the decoded instruction.

CPU ABI **7** adds `FarOperand` to the shared encoder operand type. Rebuild
older CPU plugins against the current headers. The loader rejects plugins with
a mismatched ABI version.

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
