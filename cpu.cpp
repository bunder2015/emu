// for std::cerr std::cout std::hex
#include <iostream>

// for cpubus cpuregs
#include "cpu.h"
// for cpumem_read cpumem_write
#include "mmu.h"

using std::cerr;
using std::cout;
using std::hex;

int cpu_init(cpubus &cb, cpuregs &cr) {
    // Set the Program Counter
    cb.cpuaddrbus = 0xFFFC;
    cpumem_read(cb);
    uint8_t pclow = cb.cpudatabus;
    cb.cpuaddrbus++;
    cpumem_read(cb);
    uint8_t pchigh = cb.cpudatabus;
    cr.pc = static_cast <uint16_t> (pchigh << 8 | pclow);

    if (cr.pc < 0x8000) {
        cerr << "ERROR: CPU boot PC is not in PRG ROM memory!\n";
        return 1;
    } else {
        cout << "INFO: CPU execution will begin at 0x" << hex << cr.pc << '\n';
        return 0;
    }
}

int cpu_run(cpubus &cb, cpuregs &cr, bool &canrun) {
    cb.cpuaddrbus = cr.pc;
    cpumem_read(cb);
// TODO (chris#9#): Why doesn't std::hex like uint8_t or unsigned char
    unsigned short opcode = cb.cpudatabus;
    // Used for 2 or 3 byte instructions
    uint8_t oplow = 0x00;
    uint8_t ophigh = 0x00;
    int8_t relop = 0x00;
    // Used to check whether we need to update the CPU flags
    bool aupdatenz = false;
    bool xupdatenz = false;
    bool yupdatenz = false;

    switch (opcode) {
// TODO (chris#3#): CPU cycle accuracy
    // The following opcodes are known to exist
    case 0x00:
        // Opcode 00 - BRK (BReaK)
        cerr << "ERROR: BRK occurred at 0x" << hex << cr.pc << '\n';
        canrun = false;
        break;
    case 0x18:
        // Opcode 18 - CLC (CLear Carry flag)
        cr.c = false;
        cr.pc++;
        break;
    case 0x29:
        // Opcode 29 - AND (bitwise AND with accumulator) $#nn Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.a = (cr.a & cb.cpudatabus);
        aupdatenz = true;
        cr.pc++;
        break;
    case 0x38:
        // Opcode 38 - SEC (SEt Carry flag)
        cr.c = true;
        cr.pc++;
        break;
    case 0x58:
        // Opcode 58 - CLI (CLear Interrupt disable flag)
        cr.i = false;
        cr.pc++;
        break;
    case 0x78:
        // Opcode 78 - SEI (SEt Interrupt disable flag)
        cr.i = true;
        cr.pc++;
        break;
    case 0x88:
        // Opcode 88 - DEY (CEcrement Y index register)
        cr.y--;
        yupdatenz = true;
        cr.pc++;
        break;
    case 0x8D:
        // Opcode 8D - STA (STore Accumulator) $nnnn Absolute
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        oplow = cb.cpudatabus;
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        ophigh = cb.cpudatabus;
        cb.cpuaddrbus = static_cast<uint16_t> ((ophigh << 8) | oplow);
        cb.cpudatabus = cr.a;
        cpumem_write(cb);
        cr.pc++;
        break;
    case 0x9A:
        // Opcode 9A - TXS (Transfer X index register to Stack pointer)
        cr.sp = cr.x;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0x9D:
        // Opcode 9D - STA (STore Accumulator) $nnnn,x Absolute Indexed with X
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        oplow = cb.cpudatabus;
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        ophigh = cb.cpudatabus;
        cb.cpuaddrbus = static_cast<uint16_t> (((ophigh << 8) | oplow) + cr.x);
        cb.cpudatabus = cr.a;
        cpumem_write(cb);
        cr.pc++;
        break;
    case 0xA2:
        // Opcode A2 - LDX (LoaD X index register) $#nn Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.x = cb.cpudatabus;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0xA9:
        // Opcode A9 - LDA (LoaD Accumulator) $#nn Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.a = cb.cpudatabus;
        aupdatenz = true;
        cr.pc++;
        break;
    case 0xAD:
        // Opcode AD - LDA (LoaD Accumulator) $nnnn Absolute
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        oplow = cb.cpudatabus;
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        ophigh = cb.cpudatabus;
        cb.cpuaddrbus = static_cast<uint16_t> ((ophigh << 8) | oplow);
        cpumem_read(cb);
        cr.a = cb.cpudatabus;
        cr.pc++;
        break;
    case 0xB8:
        // Opcode B8 - CLV (CLear oVerflow flag)
        cr.v = false;
        cr.pc++;
        break;
    case 0xCA:
        // Opcode CA - DEX (CEcrement X index register)
        cr.x--;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0xC8:
        // Opcode C8 - INY (INcrement Y index register)
        cr.y++;
        yupdatenz = true;
        cr.pc++;
        break;
    case 0xD0:
        // Opcode D0 - BNE (Branch Not Equal) $nn Relative
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        relop = static_cast<int8_t> (cb.cpudatabus);
        if (cr.z == false) {
            cr.pc = static_cast<uint16_t> (cr.pc + relop);
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0xD8:
        // Opcode D8 - CLD (CLear Decimal flag)
        cr.d = false;
        cr.pc++;
        break;
    case 0xE8:
        // Opcode E8 - INX (INcrement X index register)
        cr.x++;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0xEA:
        // Opcode EA - NOP (No OPeration)
        cr.pc++;
        break;
    case 0xF0:
        // Opcode F0 - BEQ (Branch if EQual) $nn Relative
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        relop = static_cast<int8_t> (cb.cpudatabus);
        if (cr.z == true) {
            cr.pc = static_cast<uint16_t> (cr.pc + relop);
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0xF8:
        // Opcode F8 - SED (SEt Decimal flag)
        cr.d = true;
        cr.pc++;
        break;

    // The following opcodes are known to not exist (including illegal opcodes)
    case 0x02: case 0x03: case 0x04: case 0x07: case 0x0B: case 0x0C: case 0x0F:
    case 0x12: case 0x13: case 0x14: case 0x17: case 0x1A: case 0x1B: case 0x1C: case 0x1F:
    case 0x22: case 0x23: case 0x27: case 0x2B: case 0x2F:
    case 0x32: case 0x33: case 0x34: case 0x37: case 0x3A: case 0x3B: case 0x3C: case 0x3F:
    case 0x42: case 0x43: case 0x44: case 0x47: case 0x4B: case 0x4F:
    case 0x52: case 0x53: case 0x54: case 0x57: case 0x5A: case 0x5B: case 0x5C: case 0x5F:
    case 0x62: case 0x63: case 0x64: case 0x67: case 0x6B: case 0x6F:
    case 0x72: case 0x73: case 0x74: case 0x77: case 0x7A: case 0x7B: case 0x7C: case 0x7F:
    case 0x80: case 0x82: case 0x83: case 0x87: case 0x89: case 0x8B: case 0x8F:
    case 0x92: case 0x93: case 0x97: case 0x9B: case 0x9C: case 0x9E: case 0x9F:
    case 0xA3: case 0xA7: case 0xAB: case 0xAF:
    case 0xB2: case 0xB3: case 0xB7: case 0xBB: case 0xBF:
    case 0xC2: case 0xC3: case 0xC7: case 0xCB: case 0xCF:
    case 0xD2: case 0xD3: case 0xD4: case 0xD7: case 0xDA: case 0xDB: case 0xDC: case 0xDF:
    case 0xE2: case 0xE3: case 0xE7: case 0xEB: case 0xEF:
    case 0xF2: case 0xF3: case 0xF4: case 0xF7: case 0xFA: case 0xFB: case 0xFC: case 0xFF:
        cerr << "ERROR: CPU opcode at 0x" << hex << cb.cpuaddrbus << " does not exist: " << hex << opcode << '\n';
        canrun = false;
        return 1;
    default:
// TODO (chris#2#): More CPU opcodes, remove giant case case case block above
        cerr << "ERROR: CPU opcode at 0x" << hex << cb.cpuaddrbus << " not yet implemented: " << hex << opcode << '\n';
        canrun = false;
        return 1;
    }

    // CPU flag update routine
    if (aupdatenz == true) {
        if (cr.a > 0x7F) {
            cr.n = true;
        } else {
            cr.n = false;
        }

        if (cr.a == 0x00) {
            cr.z = true;
        } else {
            cr.z = false;
        }
    } else if (xupdatenz == true) {
        if (cr.x > 0x7F) {
            cr.n = true;
        } else {
            cr.n = false;
        }

        if (cr.x == 0x00) {
            cr.z = true;
        } else {
            cr.z = false;
        }
    } else if (yupdatenz == true) {
        if (cr.y > 0x7F) {
            cr.n = true;
        } else {
            cr.n = false;
        }

        if (cr.y == 0x00) {
            cr.z = true;
        } else {
            cr.z = false;
        }
    }

    return 0;
}
