#include <algorithm>    // for std::fill
#include <cstring>      // for memcpy
#include <iostream>     // for std::cerr std::hex

#include "cpu.h"        // for cpubus
#include "memory.h"     // for consolewram consolevram prgrom chrrom ppuoamram ppupaletteram
#include "ppu.h"        // for ppubus
#include "rom.h"        // for rom_ingest rom_headerparse romheader

using std::cerr;
using std::fill;
using std::hex;

cpubus cb;
romheader rh;
ppubus pb;

void consoleram_init() {
    fill (consolewram, (consolewram + 0x800), 0xFF);    // Initialize console WRAM
    fill (consolevram, (consolevram + 0x800), 0xFF);    // Initialize console VRAM
    fill (ppuoamram, (ppuoamram + 0x100), 0xFF);        // Initialize PPU internal OAM RAM
    fill (ppupaletteram, (ppupaletteram + 0x20), 0xFF); // Initialize PPU internal palette RAM
}

int cpumem_read() {
    switch (rh.mapper) {
    case 0:
        /*  iNES mapper 0 (aka NROM)
        *     2kb console WRAM mapped to CPU 0x0000, 0x800, 0x1000, 0x1800
        *     32kb PRG ROM mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
        */
        if (cb.cpuaddrbus <= 0x1FFF) {                              // If we are reading the console WRAM
            cb.cpudatabus = *(consolewram + cb.cpuaddrbus % 0x800); // put the data on the bus for the CPU to read
        } else if (cb.cpuaddrbus >= 0x8000) {                       // If we are reading from cartridge ROM
            if (rh.prgromsize == 32768) {                           // and we are a 32kb PRG ROM
                cb.cpudatabus = *(prgrom + (cb.cpuaddrbus % 0x8000));   // put the data on the bus for the CPU to read
            } else {
                uint16_t offsetaddr = cb.cpuaddrbus % 0x8000;       // Determine whether we are using 0x8000 or 0xC000
                if (offsetaddr <= 0x3FFF) {                         // If we used 0x8000
                    cb.cpudatabus = *(prgrom + offsetaddr);         // put the 0x8000 data on the bus for the CPU to read
                } else {                                            // If we used 0xC000
                    cb.cpudatabus = *(prgrom + (offsetaddr - 0x4000));  // put the mirrored 0x8000 data on the bus for the CPU to read
                }

            }

        } else {
            cerr << "ERROR: CPU tried to read open bus, not yet implemented: 0x" << hex << cb.cpuaddrbus << '\n';
            return 1;
        }

        break;
    default:
// TODO (chris#4#): More memory mappers
        cerr << "ERROR: Memory mapper not yet implemented\n";       // We should never get here if mmu_init does its job
        return 1;
    }

    return 0;
}

int cpumem_write() {
    switch (rh.mapper) {
    case 0:
        /*  iNES mapper 0 (aka NROM)
        *     2kb console WRAM mapped to CPU 0x0000, 0x800, 0x1000, 0x1800
        */
        if (cb.cpuaddrbus <= 0x1FFF) {                              // If we are writing to console WRAM
            *(consolewram + cb.cpuaddrbus % 0x800) = cb.cpudatabus; // take the data off the bus and write it to the memory block
        } else {
            cerr << "ERROR: CPU tried to write to non-writable memory: 0x" << hex << cb.cpuaddrbus << '\n';
            return 1;
        }

        break;
    default:
// TODO (chris#4#): More memory mappers
        cerr << "ERROR: Memory mapper not yet implemented\n";       // We should never get here if mmu_init does its job
        return 1;
    }

    return 0;
}

int ppumem_read() {
    switch (rh.mapper) {
    case 0:
        if (pb.ppuaddrbus <= 0x1FFF) {                  // CHR ROM (pattern table)
            pb.ppudatabus = *(chrrom + pb.ppuaddrbus);
        } else if (pb.ppuaddrbus <= 0x3EFF) {           // Console VRAM
            pb.ppudatabus = *(consolevram + pb.ppuaddrbus % 0x100);
        } else if (pb.ppuaddrbus <= 0x3FFF) {           // PPU internal palette RAM
            pb.ppudatabus = *(ppupaletteram + pb.ppuaddrbus % 0x20);
        } else {
            cerr << "ERROR: PPU tried to read outside accessible memory: 0x" << hex << pb.ppuaddrbus << '\n';
            return 1;
        }
        break;
    default:
// TODO (chris#4#): More memory mappers
        cerr << "ERROR: Memory mapper not yet implemented\n";
        return 1;
    }

    return 0;
}

int ppumem_write() {
    switch (rh.mapper) {
    case 0:
        if (pb.ppuaddrbus <= 0x1FFF) {                  // CHR ROM (pattern table)
            cerr << "ERROR: PPU tried to write to non-writable memory: 0x" << hex << pb.ppuaddrbus << '\n';
        } else if (pb.ppuaddrbus <= 0x3EFF) {           // Console VRAM
            *(consolevram + pb.ppuaddrbus % 0x100) = pb.ppudatabus;
        } else if (pb.ppuaddrbus <= 0x3FFF) {           // PPU internal palette RAM
            *(ppupaletteram + pb.ppuaddrbus % 0x20) = pb.ppudatabus;
        } else {
            cerr << "ERROR: PPU tried to write outside accessible memory: 0x" << hex << pb.ppuaddrbus << '\n';
            return 1;
        }
        break;
    default:
// TODO (chris#4#): More memory mappers
        cerr << "ERROR: Memory mapper not yet implemented\n";
        return 1;
    }

    return 0;
}

int mmu_init(char *romfile) {
    if ((rom_ingest(romfile, rh) == 0) && (rom_headerparse(rh) == 0)) { // Load the ROM into memory
        consoleram_init();

        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     32kb PRG ROM to be mapped to CPU 0x8000 (if 16kb PRG ROM, mapped to 0x8000 and 0xC000)
            *     8kb CHR ROM to be mapped to PPU 0x0000
            */
            prgrom = new uint8_t[rh.prgromsize];
            memcpy(prgrom, (rh.rombuffer + 16), rh.prgromsize);     // Skip the header and copy PRG ROM data to its own container
            chrrom = new uint8_t[rh.chrromsize];
            memcpy(chrrom, (rh.rombuffer + 16 + rh.prgromsize), rh.chrromsize); // Skip the header and PRG ROM and copy CHR ROM to its own container
            break;
        default:
// TODO (chris#4#): More memory mappers
            cerr << "ERROR: Memory mapper not yet implemented\n";
            return 1;
        }

        delete[] rh.rombuffer;  // Free rombuffer, we don't need it anymore after this point

// FIXME (chris#6#): Remove CPU/PPU testing code
        cb.cpuaddrbus = 0x8002;
        cpumem_read();
        cb.cpuaddrbus = 0x0000;
        cpumem_write();

        pb.ppuaddrbus = 0x0003;
        ppumem_read();
        pb.ppuaddrbus = 0x2000;
        ppumem_write();

        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
