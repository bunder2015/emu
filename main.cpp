#include <iostream>     // for std::cerr

#include "cpu.h"        // for cpubus cpu_init
#include "mmu.h"        // for mmu_init
#include "ppu.h"        // for ppubus ppu_init

using std::cerr;

cpubus cb;
ppubus pb;

int main(int argc, char *argv[]) {
    char *romfile;          // ROM filename for input

    if (argc < 2) {         // Argument count check
        cerr << "ERROR: ROM filename not given.\n";
        return 1;
    } else if (argc > 2) {
        cerr << "ERROR: Too many arguments.\n";
        return 1;
    } else {
        romfile = argv[1];  // Try to load this as a ROM file
    }

    if ((mmu_init(romfile) == 0)  // Load ROM file and initialize memory map
            && (cpu_init(cb) == 0)    // Initialize CPU registers
            && (ppu_init(pb) == 0)) { // Initialize PPU
// TODO (chris#2#): CPU/APU/PPU
// TODO (chris#3#): Memory mapped IO in CPU
// TODO (chris#7#): Bus conflicts
// TODO (chris#7#): Open bus
    } else {
        return 1;   // mmu_init failed
    }

    return 0;       // Exit
}
