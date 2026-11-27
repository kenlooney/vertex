/*
 * Copyright (C) 2026 Kenneth Looney
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include "vertex/loaders/cpuloader.h"
#include "osasm/assembler.h"
#include "kasmlang/grammar.h"


int main(int argc, char* argv[]) {
    using namespace std::string_view_literals;
    if (argc != 6 ||
        (argv[1] != "-i"sv && argv[1] != "--input"sv) ||
        (argv[3] != "-o"sv && argv[3] != "--output"sv) ||
        argv[2][0] == '\0' || argv[4][0] == '\0' || argv[5][0] == '\0') {
        std::cerr << "Usage: osasm (-i|--input) <input-file> (-o|--output) <output-file> <cpu-plugin>\n";
        return 1;
    }

    const char* cpuPlugin = argv[5];
    CpuLoader loader;
    std::string error;
    if (!loader.open(cpuPlugin, error)) {
        std::cerr << "Failed to load CPU plugin '" << cpuPlugin << "': " << error << std::endl;
        return 1;
    }
    std::cout << "CPU name: " << loader.cpu()->name() << std::endl;

    // Construct assembler and grammar
    OsasmAssembler assembler;
    KasmGrammar grammar;
    assembler.setCpu(loader.cpu());
    assembler.setGrammar(&grammar);

    Source source;

    if(!Source::load(argv[2], source)) {
        std::cerr << "Failed to open source file: " << argv[2] << std::endl;
        return 1;
    }
     std::string outputFileName = argv[4];
     if(outputFileName.empty()) {
        std::cerr << "A valid and output filename is required." << std::endl;
        return 1;
     }

    Options options;
    options.add(std::make_unique<StringOption>("input", argv[2]));
    options.add(std::make_unique<StringOption>("output", outputFileName));
    

    auto result = assembler.assemble(source, options);

    if (!loader.close()) {
        std::cerr << "Failed to close CPU plugin." << std::endl;
        return 1;
    }

    if(!result || result->failed()) {
        return 1;
    }
    return 0;
}
