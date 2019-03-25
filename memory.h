#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

unsigned char consolewram[2048];    // 2kb console WRAM (zero page, stack, etc), to be mapped at CPU 0x0000
unsigned char consolevram[2048];    // 2kb console VRAM (nametables), to be mapped at PPU 0x2000
unsigned char *prgrom;              // Cartridge PRG ROM
unsigned char *chrrom;              // Cartridge CHR ROM

#endif // MEMORY_H_INCLUDED