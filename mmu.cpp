#include "rom.h"    // for rom_ingest rom_headerparse

int mmu_init(char* romfile) {
    char* rombuffer;     // in memory copy of the entire ROM file

    if (rom_ingest(romfile, &rombuffer) == 0 && rom_headerparse(&rombuffer) == 0) {
// TODO (chris#2#): something after headerparse
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
