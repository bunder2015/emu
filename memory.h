#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

unsigned char consolewram[2048];    // 2kb console WRAM (zero page, stack, etc), mapped at CPU 0x0000, 0x800, 0x1000, 0x1800
unsigned char *prgrom;              // Cartridge PRG ROM, mapped at CPU 0x8000

unsigned char *chrrom;              // Cartridge CHR ROM, mapped at PPU 0x0000
unsigned char consolevram[2048];    // 2kb console VRAM (nametables), mapped at PPU 0x2000
unsigned char ppuoamram[256];       // 256b PPU internal OAM RAM
unsigned char ppupaletteram[32];    // 32b PPU internal palette RAM, mapped at PPU 0x3F00

#endif // MEMORY_H_INCLUDED
