#include <algorithm>    // for std::fill
#include <cstring>      // for memcpy
#include <iostream>     // for std::cerr

#include "rom.h"        // for rom_ingest rom_headerparse

using std::cerr;
using std::fill;

unsigned char consolewram[2048];    // 2kb console WRAM (zero page, stack, etc), to be mapped at CPU 0x0000
unsigned char consolevram[2048];    // 2kb console VRAM (nametables), to be mapped at PPU 0x2000
unsigned char *prgrom;              // Cartridge PRG ROM
unsigned char *chrrom;              // Cartridge CHR ROM

void consoleram_init() {
    fill (consolewram, (consolewram + 2048), 0xFF);  // Initialize console WRAM
    fill (consolevram, (consolevram + 2048), 0xFF);  // Initialize console VRAM
}

int mmu_init(char *romfile) {
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
        consoleram_init();

        switch (mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     No mapper hardware
            *     32kb PRG ROM to be mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
            *     8kb CHR ROM to be mapped to PPU 0x0000
            */
            prgrom = new unsigned char[prgromsize];
            memcpy(prgrom, (rombuffer + 16), prgromsize);                 // Skip the header and copy PRG ROM data to its own container
            chrrom = new unsigned char[chrromsize];
            memcpy(chrrom, (rombuffer + 16 + prgromsize), chrromsize);    // Skip the header and PRG ROM and copy CHR ROM to its own container
// TODO (chris#1#): Nametable mirroring
            break;
        default:
// TODO (chris#4#): More memory mappers
            cerr << "ERROR: Memory mapper not yet implemented\n";
            return 1;
        }

        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
