#include <iostream>     // for std::cerr std::cout

#include "cpu.h"        // for cpubus cpu_init
#include "mmu.h"        // for mmu_init
#include "ppu.h"        // for ppubus ppu_init

using std::cerr;
using std::cout;

cpubus cb;
ppubus pb;

int main(int argc, char *argv[]) {
    // ROM filename for input
    char *romfile;

    // Argument count check
    if (argc < 2) {
        cerr << "ERROR: ROM filename not given!\n";
        return 1;
    } else if (argc > 2) {
        cerr << "ERROR: Too many arguments!\n";
        return 1;
    } else {
        // Try to load this as a ROM file
        romfile = argv[1];
        cout << "INFO: Loading ROM file: " << romfile << '\n';
    }

    /*  Load ROM file and initialize memory map
    *   Initialize CPU registers
    *   Initialize PPU
    */
    if ((mmu_init(romfile) == 0)
            && (cpu_init(cb) == 0)
            && (ppu_init(pb) == 0)) {
// TODO (chris#2#): CPU/APU/PPU
// TODO (chris#3#): Memory mapped IO in CPU
// TODO (chris#7#): Bus conflicts
// TODO (chris#7#): Open bus
    } else {
        // mmu_init cpu_init ppu_init failed
        return 1;
    }

    // Exit
    return 0;
}
