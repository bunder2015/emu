#include <iostream>     // for std::cout

#include "mmu.h"        // for mmu_init

using std::cout;

int main(int argc, char* argv[]) {
    char* romfile;          // ROM filename for input

    if (argc < 2) {
        cout << "ERROR: ROM filename not given.\n";
        return 1;
    } else if (argc > 2) {
        cout << "ERROR: Too many arguments.\n";
        return 1;
    } else {
        romfile = argv[1];
    }

    if (mmu_init(romfile) == 0) { // load ROM file and initialize memory map
// TODO (chris#6#): CPU/APU/PPU
    } else {
        return 1;
    }

    // end
    return 0;
}
