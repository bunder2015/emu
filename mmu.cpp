#include <algorithm>    // for std::fill
#include <cstring>      // for memcpy
#include <iostream>     // for std::cerr std::hex

#include "cpu.h"        // for cpubus
#include "rom.h"        // for rom_ingest rom_headerparse romheader
#include "memory.h"     // for consolewram consolevram prgrom chrrom

using std::cerr;
using std::fill;
using std::hex;

int cpumem_read(cpubus cb, romheader rh) {
    switch (rh.mapper) {
    case 0:
        /*  iNES mapper 0 (aka NROM)
        *     2kb console WRAM mapped to CPU 0x0000
        *     32kb PRG ROM mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
        */
        if (cb.cpuaddrbus < 0x800) {                                // If we are reading the console WRAM
            cb.cpudatabus = *(consolewram + cb.cpuaddrbus);         // put the data on the bus for the CPU to read
        } else if (cb.cpuaddrbus >= 0x8000) {                       // If we are reading from cartridge ROM
            if (rh.prgromsize == 32768) {
                cb.cpudatabus = *(prgrom + (cb.cpuaddrbus % 0x8000));   // put the data on the bus for the CPU to read
            } else {
                unsigned short offsetaddr = cb.cpuaddrbus % 0x8000;
                if (offsetaddr < 0x4000) {
                    cb.cpudatabus = *(prgrom + offsetaddr);
                } else {
                    cb.cpudatabus = *(prgrom + (offsetaddr - 0x4000));
                }

            }

        } else {
            cerr << "ERROR: CPU tried to read open bus, not yet implemented " << hex << cb.cpuaddrbus << '\n';
            return 1;
        }

        break;
    default:
        cerr << "ERROR: Memory mapper not yet implemented\n";       // We should never get here if mmu_init does its job
        return 1;
    }

    return 0;
}

int cpumem_write(cpubus cb, romheader rh) {
    switch (rh.mapper) {
    case 0:
        /*  iNES mapper 0 (aka NROM)
        *     2kb console WRAM mapped to CPU 0x0000
        */
        if (cb.cpuaddrbus < 0x800) {                                // If we are writing to console WRAM
            *(consolewram + cb.cpuaddrbus) = cb.cpudatabus;         // take the data off the bus and write it to the memory block
        } else {
            cerr << "ERROR: CPU tried to write to non-writable memory " << hex << cb.cpuaddrbus << '\n';
            return 1;
        }

        break;
    default:
        cerr << "ERROR: Memory mapper not yet implemented\n";       // We should never get here if mmu_init does its job
        return 1;
    }

    return 0;
}

void consoleram_init() {
    fill (consolewram, (consolewram + 0x800), 0xFF);  // Initialize console WRAM
    fill (consolevram, (consolevram + 0x800), 0xFF);  // Initialize console VRAM
}

int mmu_init(char *romfile) {
    romheader rh;
    if ((rom_ingest(romfile, rh) == 0) && (rom_headerparse(rh) == 0)) { // Load the ROM into memory
        consoleram_init();

        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     No mapper hardware
            *     32kb PRG ROM to be mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
            *     8kb CHR ROM to be mapped to PPU 0x0000
            */
            prgrom = new unsigned char[rh.prgromsize];
            memcpy(prgrom, (rh.rombuffer + 16), rh.prgromsize);                 // Skip the header and copy PRG ROM data to its own container
            chrrom = new unsigned char[rh.chrromsize];
            memcpy(chrrom, (rh.rombuffer + 16 + rh.prgromsize), rh.chrromsize); // Skip the header and PRG ROM and copy CHR ROM to its own container
// TODO (chris#1#): Nametable mirroring
            break;
        default:
// TODO (chris#4#): More memory mappers
            cerr << "ERROR: Memory mapper not yet implemented\n";
            return 1;
        }

// FIXME (chris#6#): Remove CPU testing code
        cpubus cb;
        cb.cpuaddrbus = 0x8000;
        cpumem_read(cb, rh);
        return 0;
    } else {
        return 1;   // rom_ingest or rom_headerparse failed
    }
}
