#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

#include "cpu.h"    // for cpubus
#include "ppu.h"    // for ppubus
#include "rom.h"    // for romheader

int cpumem_read(cpubus cb, romheader rh);
int cpumem_write(cpubus cb, romheader rh);
int mmu_init(char *romfile);
int ppumem_read(ppubus pb, romheader rh);
int ppumem_write(ppubus pb, romheader rh);

#endif // MMU_H_INCLUDED
