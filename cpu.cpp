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
    cb.cpuaddrbus = 0xFFFD;
    cpumem_read(cb);
    uint8_t pchigh = cb.cpudatabus;
    cr.pc = static_cast <uint16_t> (pchigh << 8 | pclow);

    if (cr.pc <= 0x8000) {
        cerr << "ERROR: CPU boot PC is not in PRG ROM memory!\n";
        return 1;
    } else {
        cout << "INFO: CPU execution will begin at 0x" << hex << cr.pc << '\n';
        return 0;
    }
}

int cpu_run(cpubus &cb, cpuregs &cr) {
    cb.cpuaddrbus = cr.pc;
    cpumem_read(cb);
// TODO (chris#9#): Why doesn't std::hex like uint8_t or unsigned char
    unsigned short opcode = cb.cpudatabus;
    // Used for 3 byte instructions
    uint8_t oplow = 0x0000;
    uint8_t ophigh = 0x0000;
    // Used to check whether we need to update the CPU flags
    bool aupdatenz = false;
    bool xupdatenz = false;

    switch (opcode) {
// TODO (chris#3#): CPU cycle accuracy
    case 0x29:
        // Opcode 29 - AND (bitwise AND with accumulator) $#xx Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.a = (cr.a & cb.cpudatabus);
        aupdatenz = true;
        cr.pc++;
        break;
    case 0x78:
        // Opcode 78 - SEI (SEt Interrupt disable flag)
        cr.i = true;
        cr.pc++;
        break;
    case 0x8D:
        // Opcode 8D - STA (STore Accumulator) $xxxx Absolute
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
        // Opcode 9A - TXS (Transfer X to Stack pointer)
        cr.sp = cr.x;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0xA2:
        // Opcode A2 - LDX (LoaD X register) $#xx Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.x = cb.cpudatabus;
        xupdatenz = true;
        cr.pc++;
        break;
    case 0xA9:
        // Opcode A9 - LDA (LoaD A register) $#xx Immediate
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        cpumem_read(cb);
        cr.a = cb.cpudatabus;
        aupdatenz = true;
        cr.pc++;
        break;
    case 0xAD:
        // Opcode AD - LDA (LoaD Accumulator) $xxxx Absolute
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
    case 0xD8:
        // Opcode D8 - CLD (CLear Decimal flag)
        cr.d = false;
        cr.pc++;
        break;
    case 0xEA:
        // Opcode EA - NOP (No OPeration)
        cr.pc++;
        break;
    default:
// TODO (chris#2#): More CPU opcodes
        cerr << "ERROR: CPU opcode not yet supported: " << hex << opcode << '\n';
        return 1;
    }

    if (aupdatenz == true) {
        if (cr.a <= 0x7F) {
            cr.n = false;
        } else {
            cr.n = true;
        }

        if (cr.a == 0x00) {
            cr.z = true;
        } else {
            cr.z = false;
        }

    } else if (xupdatenz == true) {
        if (cr.x <= 0x7F) {
            cr.n = false;
        } else {
            cr.n = true;
        }

        if (cr.x == 0x80) {
            cr.z = true;
        } else {
            cr.z = false;
        }
    }

    return 0;
}
