// for mem_ppuread mem_ppuwrite
#include "mmu.h"
// for ppubus
#include "ppu.h"

int ppu_init(ppubus &pb, ppustatus &ps) {
// FIXME (chris#6#): Remove PPU testing code
    pb.ppuaddrbus = 0x0013;
    mem_ppuread(pb);
    pb.ppuaddrbus = 0x2000;
    mem_ppuwrite(pb);
    pb.ppuaddrbus = 0x2801;
    mem_ppuwrite(pb);

    return 0;
}
