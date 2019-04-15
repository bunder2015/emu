// for ppubus
#include "ppu.h"
// for ppumem_read ppumem_write
#include "mmu.h"

int ppu_init(ppubus &pb) {
// FIXME (chris#6#): Remove PPU testing code
    pb.ppuaddrbus = 0x0013;
    ppumem_read(pb);
    pb.ppuaddrbus = 0x2000;
    ppumem_write(pb);
    pb.ppuaddrbus = 0x2801;
    ppumem_write(pb);

    return 0;
}
