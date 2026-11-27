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

#pragma once
#include "vertex/common/cpu.h"
#include "vertex/plugins/plugin.h"

// Bump when the CPU plugin interface changes, including its encoder interface.
#define VERTEX_CPU_PLUGIN_ABI_VERSION 7

VERTEX_DECLARE_PLUGIN_KIND(Cpu, cpu, "CPU", VERTEX_CPU_PLUGIN_ABI_VERSION)

// Add to one source file of a CPU plugin. CpuType must derive from Cpu and be default-constructible.
#define VERTEX_REGISTER_CPU(CpuType) VERTEX_REGISTER_PLUGIN(cpu, Cpu, CpuType)
