#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

// for cpubus
#include "cpu.h"
// for ppubus
#include "ppu.h"

int mem_cpuread(cpubus &cb);
int mem_cpuwrite(cpubus &cb);
int mem_ppuread(ppubus &pb);
int mem_ppuwrite(ppubus &pb);
int mmu_init(char *romfile);

#endif // MMU_H_INCLUDED
