#include "rom.h"    // for rom_ingest rom_headerparse

int mmu_init(char* romfile) {
    char* rombuffer;        // in memory copy of the entire ROM file
    int prgromsize = 0;     // size of PRG ROM (in multiples of 16kb)
    int chrromsize = 0;     // size of CHR ROM (in multiples of 8kb)
    int mapper = 0;         // iNES mapper number

    if (rom_ingest(romfile, &rombuffer) == 0                                    // Load the ROM into memory
            && rom_headerparse(&rombuffer, &prgromsize, &chrromsize) == 0) {    // and parse the header
// TODO (chris#5#): something after headerparse
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
