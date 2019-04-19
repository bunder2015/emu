// for std::cerr std::cout
#include <iostream>

// for cpubus cpu_init
#include "cpu.h"
// for mmu_init
#include "mmu.h"
// for ppubus ppu_init
#include "ppu.h"

using std::cerr;
using std::cout;

cpubus cb;
cpuregs cr;
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
            && (cpu_init(cb, cr) == 0)
            && (ppu_init(pb) == 0)) {
// TODO (chris#2#): CPU/APU/PPU
        while (true) {
            cpu_run(cb, cr);
        }
// TODO (chris#3#): Memory mapped IO in CPU (PPU/APU registers)
// TODO (chris#7#): Bus conflicts
// TODO (chris#7#): Open bus
    } else {
        // mmu_init cpu_init ppu_init failed
        return 1;
    }

    // Exit
    return 0;
}
