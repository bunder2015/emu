#include "rom.h"    // for rom_ingest rom_headerparse

int mmu_init(char* romfile) {
    char* rombuffer;                // In memory copy of the entire ROM file
    unsigned short mapper = 0;      // iNES mapper number
    unsigned long prgromsize = 0;   // Size of PRG ROM
    unsigned long prgramsize = 0;   // Size of PRG RAM (WRAM + save WRAM)
    unsigned long chrromsize = 0;   // Size of CHR ROM
    unsigned long chrramsize = 0;   // Size of CHR RAM (VRAM)
    bool batterypresent = false;    // PRG RAM save battery presence toggle
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
// TODO (chris#5#): Memory mappers
// TODO (chris#7#): Bus conflicts
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
