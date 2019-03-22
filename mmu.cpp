#include <algorithm>    // for std::fill

#include "rom.h"        // for rom_ingest rom_headerparse

using std::fill;

unsigned char consolewram[0x800];   // 2kb console WRAM (zero page, stack, etc), to be mapped at CPU 0x0000
unsigned char consolevram[0x800];   // 2kb console VRAM (nametables), to be mapped at PPU 0x2000

int mmu_init(char* romfile) {
    char* rombuffer;                // In memory copy of the entire ROM file
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

    if (rom_ingest(romfile, &rombuffer) == 0    // Load the ROM into memory
            && rom_headerparse(&rombuffer,      // and parse the header
                               &mapper,
                               &prgromsize,
                               &prgramsize,
                               &chrromsize,
                               &chrramsize,
                               &batterypresent,
                               &mirrormode,
                               &fourscreenmode,
                               &tvsystem,
                               &busconflicts) == 0) {
        fill (consolewram+0x000, consolewram+0x800, 0xFF);    // Initialize console WRAM
        fill (consolevram+0x000, consolevram+0x800, 0xFF);    // Initialize console VRAM
// TODO (chris#5#): Memory mappers
// TODO (chris#7#): Bus conflicts
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
