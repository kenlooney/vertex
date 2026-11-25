#pragma once
#include "vertex/common/cpu.h"
#include "vertex/plugins/plugin.h"

// Bump whenever Cpu's layout or virtual interface changes so stale plugins are rejected.
#define VERTEX_CPU_PLUGIN_ABI_VERSION 2

VERTEX_DECLARE_PLUGIN_KIND(Cpu, cpu, "CPU", VERTEX_CPU_PLUGIN_ABI_VERSION)

// Add to one source file of a CPU plugin. CpuType must derive from Cpu and be default-constructible.
#define VERTEX_REGISTER_CPU(CpuType) VERTEX_REGISTER_PLUGIN(cpu, Cpu, CpuType)
