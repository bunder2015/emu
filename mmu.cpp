// for std::fill
#include <algorithm>
// for memcpy
#include <cstring>
// for std::cerr std::hex
#include <iostream>

// for cpubus
#include "cpu.h"
// for consolewram consolevram prgrom chrrom ppuoamram ppupaletteram
#include "memory.h"
// for ppubus
#include "ppu.h"
// for rom_ingest rom_headerparse romheader
#include "rom.h"

using std::cerr;
using std::fill;
using std::hex;

romheader rh;

void consoleram_init() {
    /*  Initialize console WRAM
    *   Initialize console VRAM
    *   Initialize PPU internal palette RAM
    *   Initialize PPU internal OAM RAM (aka SPR RAM)
    */
    fill (consolewram, (consolewram + 0x800), 0xFF);
    fill (consolevram, (consolevram + 0x800), 0xFF);
    fill (ppupaletteram, (ppupaletteram + 0x20), 0xFF);
    fill (ppuoamram, (ppuoamram + 0x100), 0xFF);
}

int mem_cpuread(cpubus &cb) {
    switch (cb.cpuaddrbus) {
// TODO (chris#4#): PPU/APU registers
    case 0x2000:
        // PPUCTRL
    case 0x2001:
        // PPUMASK
    case 0x2002:
        // PPUSTATUS
    case 0x2003:
        // OAMADDR
    case 0x2004:
        // OAMDATA
    case 0x2005:
        // PPUSCROLL
    case 0x2006:
        // PPUADDR
    case 0x2007:
        // PPUDATA
    case 0x4000:
        // SQ1VOL
    case 0x4001:
        // SQ1SWEEP
    case 0x4002:
        // SQ1LOW
    case 0x4003:
        // SQ1HIGH
    case 0x4004:
        // SQ2VOL
    case 0x4005:
        // SQ2SWEEP
    case 0x4006:
        // SQ2LOW
    case 0x4007:
        // SQ2HIGH
    case 0x4008:
        // TRILINEAR
    case 0x400A:
        // TRILOW
    case 0x400B:
        // TRIHIGH
    case 0x400C:
        // NOISEVOL
    case 0x400E:
        // NOISELOW
    case 0x400F:
        // NOISEHIGH
    case 0x4010:
        // DMCFREQ
    case 0x4011:
        // DMCRAW
    case 0x4012:
        // DMCSTART
    case 0x4013:
        // DMCLENGTH
    case 0x4014:
        // OAMDMA
    case 0x4015:
        // SNDCHAN
    case 0x4016:
        // JOY1/STROBE
    case 0x4017:
        // JOY2/APUFRAME
        break;
    default:
        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     2kb console WRAM mapped to CPU 0x0000, 0x800, 0x1000, 0x1800
            *     32kb PRG ROM mapped to CPU 0x8000 (if 16kb PRG ROM, map to 0x8000 and 0xC000)
            */
            if (cb.cpuaddrbus <= 0x1FFF) {
                /*  If we are reading the console WRAM
                *   put the data on the bus for the CPU to read
                */
                cb.cpudatabus = *(consolewram + (cb.cpuaddrbus % 0x800));
            } else if (cb.cpuaddrbus >= 0x8000) {
                if (rh.prgromsize == 32768) {
                    /*  If we are reading from cartridge ROM
                    *   and we are a 32kb PRG ROM
                    *   put the data on the bus for the CPU to read
                    */
                    cb.cpudatabus = *(prgrom + (cb.cpuaddrbus % 0x8000));
                } else {
                    // Determine whether we are using 0x8000 or 0xC000
                    uint16_t offsetaddr = cb.cpuaddrbus % 0x8000;
                    if (offsetaddr <= 0x3FFF) {
                        /*  If we used 0x8000
                        *   put the 0x8000 data on the bus for the CPU to read
                        */
                        cb.cpudatabus = *(prgrom + offsetaddr);
                    } else {
                        /*  If we used 0xC000
                        *   put the mirrored 0x8000 data on the bus for the CPU to read
                        */
                        cb.cpudatabus = *(prgrom + (offsetaddr - 0x4000));
                    }

                }

            } else {
                cerr << "ERROR: CPU tried to read open bus, not yet implemented: 0x" << hex << cb.cpuaddrbus << '\n';
                return 1;
            }

            break;
        default:
// TODO (chris#4#): More memory mappers
            // We should never get here if mmu_init does its job
            cerr << "ERROR: Memory mapper not yet implemented!\n";
            return 1;
        }
    }
    return 0;
}

int mem_cpuwrite(cpubus &cb) {
    switch (cb.cpuaddrbus) {
// TODO (chris#4#): PPU/APU registers
    case 0x2000:
        // PPUCTRL
    case 0x2001:
        // PPUMASK
    case 0x2002:
        // PPUSTATUS
    case 0x2003:
        // OAMADDR
    case 0x2004:
        // OAMDATA
    case 0x2005:
        // PPUSCROLL
    case 0x2006:
        // PPUADDR
    case 0x2007:
        // PPUDATA
    case 0x4000:
        // SQ1VOL
    case 0x4001:
        // SQ1SWEEP
    case 0x4002:
        // SQ1LOW
    case 0x4003:
        // SQ1HIGH
    case 0x4004:
        // SQ2VOL
    case 0x4005:
        // SQ2SWEEP
    case 0x4006:
        // SQ2LOW
    case 0x4007:
        // SQ2HIGH
    case 0x4008:
        // TRILINEAR
    case 0x400A:
        // TRILOW
    case 0x400B:
        // TRIHIGH
    case 0x400C:
        // NOISEVOL
    case 0x400E:
        // NOISELOW
    case 0x400F:
        // NOISEHIGH
    case 0x4010:
        // DMCFREQ
    case 0x4011:
        // DMCRAW
    case 0x4012:
        // DMCSTART
    case 0x4013:
        // DMCLENGTH
    case 0x4014:
        // OAMDMA
    case 0x4015:
        // SNDCHAN
    case 0x4016:
        // JOY1/STROBE
    case 0x4017:
        // JOY2/APUFRAME
        break;
    default:
        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     2kb console WRAM mapped to CPU 0x0000, 0x800, 0x1000, 0x1800
            */
            if (cb.cpuaddrbus <= 0x1FFF) {
                /*  If we are writing to console WRAM
                *   take the data off the bus and write it to the memory block
                */
                *(consolewram + (cb.cpuaddrbus % 0x800)) = cb.cpudatabus;
            } else {
                cerr << "ERROR: CPU tried to write to non-writable memory: 0x" << hex << cb.cpuaddrbus << '\n';
                return 1;
            }

            break;
        default:
    // TODO (chris#4#): More memory mappers
            // We should never get here if mmu_init does its job
            cerr << "ERROR: Memory mapper not yet implemented!\n";
            return 1;
        }
    }
    return 0;
}

int mem_ppuread(ppubus &pb) {
    switch (rh.mapper) {
    case 0:
        /*  iNES mapper 0 (aka NROM)
        *     8kb cartridge CHR ROM, mapped at PPU 0x0000
        *     2kb console VRAM (nametables), mapped at PPU 0x2000 (mirrored horizontally or vertically)
        *     32b PPU internal palette RAM, mapped at PPU 0x3F00
        */
        if (pb.ppuaddrbus <= 0x1FFF) {
            /*  If we are reading the CHR ROM
            *   put the data on the bus for the CPU to read
            */
            pb.ppudatabus = *(chrrom + pb.ppuaddrbus);
        } else if (pb.ppuaddrbus <= 0x3EFF) {
            /*  If we are reading from the console VRAM
            *   we first deduct the first upper mirror
            */
            uint16_t offsetaddr = 0x0000;
            if (pb.ppuaddrbus >= 0x3000) {
                offsetaddr = static_cast<uint16_t> (pb.ppuaddrbus - 0x1000);
            } else {
                offsetaddr = pb.ppuaddrbus;
            }

            if (rh.mirrormode == 0) {
                /* If we are horizontal mirroring
                *  we then deduct the nametable's mirror
                */
                if (offsetaddr >= 0x2400) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x400);
                } else if (offsetaddr >= 0x2C00) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x400);
                }
                // Put the data on the bus for the PPU to read
                pb.ppudatabus = *(consolewram + (offsetaddr % 0x2000));
            } else {
                /* If we are vertical mirroring
                *  we then deduct both nametable's mirrors
                */
                if (offsetaddr >= 0x2800) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x800);
                }
                // Put the data on the bus for the PPU to read
                pb.ppudatabus = *(consolewram + (offsetaddr % 0x2000));
            }

        } else if (pb.ppuaddrbus <= 0x3FFF) {
            /* If we are reading from the PPU internal palette RAM
            *  Put the 32 bit mirrored $3F00 data on the bus
            */
            pb.ppudatabus = *(ppupaletteram + (pb.ppuaddrbus % 0x20));
        } else {
            cerr << "ERROR: PPU tried to read outside accessible memory: 0x" << hex << pb.ppuaddrbus << '\n';
            return 1;
        }

        break;
    default:
// TODO (chris#4#): More memory mappers
        // We should never get here if mmu_init does its job
        cerr << "ERROR: Memory mapper not yet implemented!\n";
        return 1;
    }

    return 0;
}

int mem_ppuwrite(ppubus &pb) {
    switch (rh.mapper) {
    case 0:
        if (pb.ppuaddrbus <= 0x1FFF) {
            // CHR ROM (pattern table)
            cerr << "ERROR: PPU tried to write to non-writable memory: 0x" << hex << pb.ppuaddrbus << '\n';
            return 1;
        } else if (pb.ppuaddrbus <= 0x3EFF) {
            // Console VRAM
            uint16_t offsetaddr = 0x0000;
            if (pb.ppuaddrbus >= 0x3000) {
                offsetaddr = static_cast<uint16_t> (pb.ppuaddrbus - 0x1000);
            } else {
                offsetaddr = pb.ppuaddrbus;
            }

            if (rh.mirrormode == 0) {
                // Horizontal
                if (offsetaddr >= 0x2400) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x400);
                } else if (offsetaddr >= 0x2C00) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x400);
                }

                *(consolevram + (offsetaddr % 0x2000)) = pb.ppudatabus;
            } else {
                // Vertical
                if (offsetaddr >= 0x2800) {
                    offsetaddr = static_cast<uint16_t> (offsetaddr - 0x800);
                }

                *(consolevram + (offsetaddr % 0x2000)) = pb.ppudatabus;
            }

        } else if (pb.ppuaddrbus <= 0x3FFF) {
            // PPU internal palette RAM
            *(ppupaletteram + (pb.ppuaddrbus % 0x20)) = pb.ppudatabus;
        } else {
            cerr << "ERROR: PPU tried to write outside accessible memory: 0x" << hex << pb.ppuaddrbus << '\n';
            return 1;
        }

        break;
    default:
// TODO (chris#4#): More memory mappers
        // We should never get here if mmu_init does its job
        cerr << "ERROR: Memory mapper not yet implemented!\n";
        return 1;
    }

    return 0;
}

int mmu_init(char *romfile) {
    /*  Load the ROM into memory
    *   and parse the header
    */
    if ((rom_ingest(romfile, rh) == 0)
            && (rom_headerparse(rh) == 0)) {
        consoleram_init();

        switch (rh.mapper) {
        case 0:
            /*  iNES mapper 0 (aka NROM)
            *     32kb PRG ROM to be mapped to CPU 0x8000 (if 16kb PRG ROM, mapped to 0x8000 and 0xC000)
            *     8kb CHR ROM to be mapped to PPU 0x0000
            *     Horizontal or vertical mirroring
            */
            if (rh.prgramsize != 0) {
// TODO (chris#9#): Family BASIC does support PRG RAM, but sticking to NTSC-U for now
                // This should also cover battery backed PRG RAM
                cerr << "ERROR: Mapper 0 does not support PRG RAM!\n";
                return 1;
            }

            if (rh.chrramsize != 0) {
                cerr << "ERROR: Mapper 0 does not support CHR RAM!\n";
                return 1;
            }

            if (rh.fourscreenmode == true) {
                cerr << "ERROR: Mapper 0 does not support four screen mirroring!\n";
                return 1;
            }

            prgrom = new uint8_t[rh.prgromsize];
            // Skip the header and copy PRG ROM data to its own container
            memcpy(prgrom, (rh.rombuffer + 16), rh.prgromsize);
            chrrom = new uint8_t[rh.chrromsize];
            // Skip the header and PRG ROM and copy CHR ROM to its own container
            memcpy(chrrom, (rh.rombuffer + 16 + rh.prgromsize), rh.chrromsize);
            break;
        default:
// TODO (chris#4#): More memory mappers
            cerr << "ERROR: Memory mapper not yet implemented!\n";
            return 1;
        }

        // We don't need rombuffer anymore
        delete[] rh.rombuffer;

        return 0;
    } else {
        // rom_ingest rom_headerparse failed
        return 1;
    }
}
