#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

// 2kb console WRAM (zero page, stack, etc), mapped at CPU 0x0000, 0x800, 0x1000, 0x1800
uint8_t consolewram[2048];
// Cartridge PRG ROM, mapped at CPU 0x8000
uint8_t *prgrom;

// Cartridge CHR ROM, mapped at PPU 0x0000
uint8_t *chrrom;
// 2kb console VRAM (nametables), mapped at PPU 0x2000 (mirrored horizontally or vertically)
uint8_t consolevram[2048];
// 32b PPU internal palette RAM, mapped at PPU 0x3F00
uint8_t ppupaletteram[32];
// 256b PPU internal OAM RAM (aka SPR RAM), not mapped to PPU, accessible to CPU via memory-mapped registers
uint8_t ppuoamram[256];

#endif // MEMORY_H_INCLUDED
