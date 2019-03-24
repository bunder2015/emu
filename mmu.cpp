#include <algorithm>    // for std::fill
#include <cstring>      // for memcpy
#include <iostream>     // for std::cerr

#include "rom.h"        // for rom_ingest rom_headerparse
#include "mmu.h"

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
    romheader rh;
    if ((rom_ingest(romfile, rh) == 0) && (rom_headerparse(rh) == 0)) { // Load the ROM into memory
        consoleram_init();

        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     No mapper hardware
            *     32kb PRG ROM to be mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
            *     8kb CHR ROM to be mapped to PPU 0x0000
            */
            prgrom = new unsigned char[rh.prgromsize];
            memcpy(prgrom, (rh.rombuffer + 16), rh.prgromsize);                 // Skip the header and copy PRG ROM data to its own container
            chrrom = new unsigned char[rh.chrromsize];
            memcpy(chrrom, (rh.rombuffer + 16 + rh.prgromsize), rh.chrromsize);    // Skip the header and PRG ROM and copy CHR ROM to its own container
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
