#include "rom.h"    // for rom_ingest rom_headerparse

int mmu_init(char* romfile) {
    char* rombuffer;        // In memory copy of the entire ROM file
    int prgromsize = 0;     // Size of PRG ROM (in multiples of 16kb)
    int prgramsize = 0;     // PRG RAM size (in multiples of 8kb << 1)
    int chrromsize = 0;     // Size of CHR ROM (in multiples of 8kb)
    int chrramsize = 0;     // Size of CHR RAM
    int mirrormode = 0;     // Horizontal/Vertical mirroring toggle
    int batterypresence = 0; // PRG RAM presence toggle
    int fourscreenmode = 0; // Four screen mirroring toggle

    int tvsystem = 0;       // TV system (NTSC/PAL)
    int mapper = 0;         // iNES mapper number

    if (rom_ingest(romfile, &rombuffer) == 0                            // Load the ROM into memory
            && rom_headerparse(&rombuffer, &prgromsize, &prgramsize,    // and parse the header
                               &chrromsize, &chrramsize, &mirrormode,
                               &batterypresence, &fourscreenmode, &tvsystem) == 0) {
// TODO (chris#5#): something after headerparse
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
