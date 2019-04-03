#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

extern struct romheader {
    char *rombuffer;                // In memory copy of the entire ROM file
    uint16_t mapper = 0;            // iNES mapper number (V1 = 0-255, V2 = 0-4095)
    uint32_t prgromsize = 0;        // Size of cartridge PRG ROM
    uint32_t prgramsize = 0;        // Size of cartridge PRG RAM (WRAM + save WRAM)
    uint32_t chrromsize = 0;        // Size of cartridge CHR ROM
    uint32_t chrramsize = 0;        // Size of cartridge CHR RAM (VRAM)
    bool batterypresent = false;    // Cartridge PRG RAM save battery presence toggle
    bool mirrormode = false;        // Horizontal/Vertical mirroring toggle (true = vertical)
    bool fourscreenmode = false;    // Four screen mirroring toggle
    bool tvsystem = false;          // NTSC/PAL TV system toggle (true = PAL)
    bool busconflicts = false;      // Bus conflicts
}romheader_t;

const uint8_t headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A
const uint8_t headerempty[4] = { 0x00, 0x00, 0x00, 0x00 }; // Four empty bytes

int rom_ingest(char *romfile, romheader &rh);
int rom_headerparse(romheader &rh);

#endif // ROM_H_INCLUDED
