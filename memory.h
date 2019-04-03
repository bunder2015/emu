#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

uint8_t consolewram[2048];    // 2kb console WRAM (zero page, stack, etc), mapped at CPU 0x0000, 0x800, 0x1000, 0x1800
uint8_t *prgrom;              // Cartridge PRG ROM, mapped at CPU 0x8000

uint8_t *chrrom;              // Cartridge CHR ROM, mapped at PPU 0x0000
uint8_t consolevram[2048];    // 2kb console VRAM (nametables), mapped at PPU 0x2000
uint8_t ppupaletteram[32];    // 32b PPU internal palette RAM, mapped at PPU 0x3F00

uint8_t ppuoamram[256];       // 256b PPU internal OAM RAM (aka SPR RAM), not mapped to PPU, accessible to CPU via memory-mapped registers

#endif // MEMORY_H_INCLUDED
