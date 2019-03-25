#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

extern struct romheader {
    char *rombuffer;                // In memory copy of the entire ROM file
    unsigned short mapper = 0;      // iNES mapper number
    unsigned long prgromsize = 0;   // Size of cartridge PRG ROM
    unsigned long prgramsize = 0;   // Size of cartridge PRG RAM (WRAM + save WRAM)
    unsigned long chrromsize = 0;   // Size of cartridge CHR ROM
    unsigned long chrramsize = 0;   // Size of cartridge CHR RAM (VRAM)
    bool batterypresent = false;    // Cartridge PRG RAM save battery presence toggle
    bool mirrormode = false;        // Horizontal/Vertical mirroring toggle (true = vertical)
    bool fourscreenmode = false;    // Four screen mirroring toggle
    bool tvsystem = false;          // NTSC/PAL TV system toggle (true = PAL)
    bool busconflicts = false;      // Bus conflicts
}rh_t;

const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A
const char headerempty[4] = { 0x00, 0x00, 0x00, 0x00 }; // Four empty bytes

int rom_ingest(char *romfile, romheader &rh);
int rom_headerparse(romheader &rh);

#endif // ROM_H_INCLUDED
