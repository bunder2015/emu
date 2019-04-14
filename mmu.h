#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "cpu.h"    // for cpubus
#include "ppu.h"    // for ppubus

int cpumem_read(cpubus &cb);
int cpumem_write(cpubus &cb);
int ppumem_read(ppubus &pb);
int ppumem_write(ppubus &pb);
int mmu_init(char *romfile);

#endif // MMU_H_INCLUDED
