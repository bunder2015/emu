#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

extern struct romheader {
    // In memory copy of the entire ROM file
    char *rombuffer;
    // iNES mapper number (V1 = 0-255, V2 = 0-4095)
    uint16_t mapper = 0;
    // Size of cartridge PRG ROM
    uint32_t prgromsize = 0;
    // Size of cartridge PRG RAM (WRAM + save WRAM)
    uint32_t prgramsize = 0;
    // Size of cartridge CHR ROM
    uint32_t chrromsize = 0;
    // Size of cartridge CHR RAM (VRAM)
    uint32_t chrramsize = 0;
    // Cartridge PRG RAM save battery presence toggle
    bool batterypresent = false;
    // Horizontal/Vertical mirroring toggle (true = vertical)
    bool mirrormode = false;
    // Four screen mirroring toggle
    bool fourscreenmode = false;
    // NTSC/PAL TV system toggle (true = PAL)
    bool tvsystem = false;
    // Bus conflicts
    bool busconflicts = false;
} romheader_t;

// Magic header bytes - "N", "E", "S", 0x1A
const uint8_t headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A };
// Four empty header bytes
const uint8_t headerempty[4] = { 0x00, 0x00, 0x00, 0x00 };

int rom_ingest(char *romfile, romheader &rh);
int rom_headerparse(romheader &rh);

#endif // ROM_H_INCLUDED
