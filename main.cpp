#include <iostream>     // for std::cerr

#include "mmu.h"        // for mmu_init

using std::cerr;

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

    if (mmu_init(romfile) == 0) {   // Load ROM file and initialize memory map
// TODO (chris#6#): CPU/APU/PPU
// TODO (chris#6#): Memory mapped IO in CPU
// TODO (chris#6#): Initialize PPU SPRRAM in PPU
// TODO (chris#7#): Bus conflicts
// TODO (chris#7#): Open bus
    } else {
        return 1;   // mmu_init failed
    }

    return 0;       // Exit
}
