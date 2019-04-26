// for std::cerr std::cout std::hex
#include <iostream>

// for cpubus cpuregs
#include "cpu.h"
// for mem_cpuread mem_cpuwrite
#include "mmu.h"

using std::cerr;
using std::cout;
using std::hex;

int cpu_init(cpubus &cb, cpuregs &cr) {
    // Set the Program Counter
    cb.cpuaddrbus = 0xFFFC;
    mem_cpuread(cb);
    uint8_t pclow = cb.cpudatabus;
    cb.cpuaddrbus++;
    mem_cpuread(cb);
    uint8_t pchigh = cb.cpudatabus;
    cr.pc = static_cast <uint16_t> (pchigh << 8 | pclow);

    if (cr.pc < 0x8000) {
        cerr << "ERROR: CPU boot PC is not in PRG ROM memory!\n";
        return 1;
    } else {
#if _DEBUG
        cout << "INFO: CPU execution will begin at 0x" << hex << cr.pc << '\n';
#endif // _DEBUG
        return 0;
    }
}

void cpu_fetchnext(cpubus &cb, cpuregs &cr) {
    cr.pc++;
    cb.cpuaddrbus = cr.pc;
    mem_cpuread(cb);
}

void cpu_updateflags(bool updaten, bool updatez, cpuregs &cr, uint8_t updateinput) {
    // CPU flag update routine
    if (updaten == true) {
        if (updateinput >= 0x80) {
            cr.n = true;
        } else {
            cr.n = false;
        }
    }

    if (updatez == true) {
        if (updateinput == 0x00) {
            cr.z = true;
        } else {
            cr.z = false;
        }
    }
}

uint8_t cpu_getflagsint(cpuregs &cr) {
    uint8_t flagsint = static_cast<uint8_t> ((
                  (cr.n << 7)
                | (cr.v << 6)
                | (cr.u << 5)
                | (cr.b << 4)
                | (cr.d << 3)
                | (cr.i << 2)
                | (cr.z << 1)
                | (cr.c) ));
    return flagsint;
}

void cpu_setflagsfromint(cpuregs &cr, uint8_t updateinput) {
    cr.c = (updateinput & 0x1);
    cr.z = ((updateinput >> 1) & 0x1);
    cr.i = ((updateinput >> 2) & 0x1);
    cr.d = ((updateinput >> 3) & 0x1);
    // The B flag can not be set externally
    //cr.b = ((updateinput >> 4) & 0x1);
    // The U flag can not be set externally
    //cr.u = ((updateinput >> 5) & 0x1);
    cr.v = ((updateinput >> 6) & 0x1);
    cr.n = ((updateinput >> 7) & 0x1);
}

int cpu_run(cpubus &cb, cpuregs &cr, bool &canrun) {
    // Used for multibyte and multiple operation instructions
    uint8_t operand2 = 0x00;
    uint8_t operand3 = 0x00;
    uint8_t operand4 = 0x00;
    uint8_t operand5 = 0x00;
    // Signed for relative operation instructions (branches)
    int8_t reloperand = 0x00;
    // Used for updating the CPU flags
    bool updatez = false;
    bool updaten = false;
    uint8_t updateinput = 0x00;

#if _DEBUG
    cout << "PC: " << hex << cr.pc << " ";
#endif // _DEBUG
    // Fetch the next instruction
    cb.cpuaddrbus = cr.pc;
    mem_cpuread(cb);
    // Used to store the opcode we just fetched
    uint8_t opcode = cb.cpudatabus;

    switch (opcode) {
// TODO (chris#3#): CPU cycle accuracy
    // The following opcodes are known to exist
    case 0x00:
        // Opcode 00 - BRK (BReaK)
#if _DEBUG
        cout << "OP: BRK" << '\t';
#endif // _DEBUG
        cerr << "ERROR: BRK occurred at 0x" << hex << cr.pc << '\n';
        /*
        *   cr.pc++;
        *   cr.pc++;
        *   cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        *   cb.cpudatabus = cr.pc;
        *   mem_cpuwrite(cb);
        *   cr.sp--;
        *   cb.cpuaddrbus++;
        *   cb.cpudatabus = (cr.pc >> 8);
        *   mem_cpuwrite(cb);
        *   cr.sp--;
        *   operand4 = cpu_getflagsint(cr);
        *   cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        *   cb.cpudatabus = operand4;
        *   mem_cpuwrite(cb);
        *   cr.sp--;
        *   cr.i = true;
        *   cb.cpuaddrbus = 0xFFFE;
        *   mem_cpuread(cb);
        *   operand4 = cb.cpudatabus;
        *   cb.cpuaddrbus++;
        *   mem_cpuread(cb);
        *   operand5 = cb.cpudatabus;
        *   cr.pc = static_cast<uint16_t> ((operand5 << 8) | operand4);
        */
        canrun = false;
        break;
    case 0x08:
        // Opcode 08 - PHP (PusH Processor status to stack)
#if _DEBUG
        cout << "OP: PHP" << '\t';
#endif // _DEBUG
        operand4 = cpu_getflagsint(cr);
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        cb.cpudatabus = operand4;
        mem_cpuwrite(cb);
        cr.sp--;
        cr.pc++;
        break;
    case 0x0A:
        // Opcode 0A - ASL (Arithmetic Shift Left) Accumulator
#if _DEBUG
        cout << "OP: ASL A" << '\t';
#endif // _DEBUG
        if (cr.a >= 0x80) {
            cr.c = true;
        } else {
            cr.c = false;
        }
        cr.a = static_cast<uint8_t> (cr.a << 1);
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x10:
        // Opcode 10 - BPL (Branch if PLus) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BPL $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.n == false) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0x18:
        // Opcode 18 - CLC (CLear Carry flag)
#if _DEBUG
        cout << "OP: CLC" << '\t';
#endif // _DEBUG
        cr.c = false;
        cr.pc++;
        break;
    case 0x20:
        // Opcode 20 - JSR (Jump to SubRoutine) Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: JSR $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        operand4 = static_cast<uint8_t> (cr.pc);
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        cb.cpudatabus = operand4;
        mem_cpuwrite(cb);
        cr.sp--;
        operand5 = static_cast<uint8_t> (cr.pc >> 8);
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        cb.cpudatabus = operand5;
        mem_cpuwrite(cb);
        cr.sp--;
        cr.pc = static_cast<uint16_t> ((operand3 << 8) | operand2);
        break;
    case 0x28:
        // Opcode 28 - PLP (PulL Processor status from stack)
#if _DEBUG
        cout << "OP: PLP" << '\t';
#endif // _DEBUG
        cr.sp++;
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        mem_cpuread(cb);
        updateinput = cb.cpudatabus;
        cpu_setflagsfromint(cr, updateinput);
        cr.pc++;
        break;
    case 0x29:
        // Opcode 29 - AND (bitwise AND with accumulator) #$nn Immediate
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: AND #$" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cr.a = (cr.a & operand2);
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x30:
        // Opcode 30 - BMI (Branch if MInus) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BMI $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.n == true) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0x38:
        // Opcode 38 - SEC (SEt Carry flag)
#if _DEBUG
        cout << "OP: SEC" << '\t';
#endif // _DEBUG
        cr.c = true;
        cr.pc++;
        break;
    case 0x48:
        // Opcode 48 - PHA (PusH Accumulator to stack)
#if _DEBUG
        cout << "OP: PHA" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        cb.cpudatabus = cr.a;
        mem_cpuwrite(cb);
        cr.sp--;
        cr.pc++;
        break;
    case 0x4A:
        // Opcode 4A - LSR (Logical Shift Right) Accumulator
#if _DEBUG
        cout << "OP: LSR A" << '\t';
#endif // _DEBUG
        if ((cr.a & 0b00000001) == 0x01) {
            cr.c = true;
        } else {
            cr.c = false;
        }
        cr.a = static_cast<uint8_t> (cr.a >> 1);
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x4C:
        // Opcode 4C - JMP (JuMP) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: JMP $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cr.pc = static_cast<uint16_t> ((operand3 << 8) | operand2);
        break;
    case 0x50:
        // Opcode 50 - BVC (Branch if oVerflow Clear) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BVC $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.v == false) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0x58:
        // Opcode 58 - CLI (CLear Interrupt disable flag)
#if _DEBUG
        cout << "OP: CLI" << '\t';
#endif // _DEBUG
        cr.i = false;
        cr.pc++;
        break;
    case 0x68:
        // Opcode 68 - PLA (PulL stack to Accumulator)
#if _DEBUG
        cout << "OP: PLA" << '\t';
#endif // _DEBUG
        cr.sp++;
        cb.cpuaddrbus = static_cast<uint16_t> (0x0100 | cr.sp);
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cr.pc++;
        break;
    case 0x6C:
        // Opcode 6C - JMP (JuMP) ($nnnn) Indirect
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: JMP ($" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << ")" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        mem_cpuread(cb);
        operand4 = cb.cpudatabus;
        cb.cpuaddrbus++;
        mem_cpuread(cb);
        operand5 = cb.cpudatabus;
        cr.pc = static_cast<uint16_t> ((operand5 << 8) | operand4);
        break;
    case 0x70:
        // Opcode 70 - BVS (Branch if oVerflow Set) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BVS $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.v == true) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0x78:
        // Opcode 78 - SEI (SEt Interrupt disable flag)
#if _DEBUG
        cout << "OP: SEI" << '\t';
#endif // _DEBUG
        cr.i = true;
        cr.pc++;
        break;
    case 0x84:
        // Opcode 84 - STY (STore Y index register) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STY $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        cb.cpudatabus = cr.y;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x85:
        // Opcode 85 - STA (STore Accumulator) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STA $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        cb.cpudatabus = cr.a;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x86:
        // Opcode 86 - STX (STore X index register) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STX $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        cb.cpudatabus = cr.x;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x88:
        // Opcode 88 - DEY (CEcrement Y index register)
#if _DEBUG
        cout << "OP: DEY" << '\t';
#endif // _DEBUG
        cr.y--;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x8A:
        // Opcode 8A - TXA (Transfer X index register to Accumulator)
#if _DEBUG
        cout << "OP: TXA" << '\t';
#endif // _DEBUG
        cr.a = cr.x;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x8C:
        // Opcode 8C - STY (STore Y index register) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STY $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        cb.cpudatabus = cr.y;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x8D:
        // Opcode 8D - STA (STore Accumulator) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STA $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        cb.cpudatabus = cr.a;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x8E:
        // Opcode 8E - STX (STore X index register) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STX $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        cb.cpudatabus = cr.x;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x90:
        // Opcode 90 - BCC (Branch if Carry Clear) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BCC $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.c == false) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0x94:
        // Opcode 94 - STY (STore Y index register) $nn,X Zero Page Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STY $" << hex << static_cast<uint16_t> (operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.x);
        cb.cpudatabus = cr.y;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x95:
        // Opcode 95 - STA (STore Accumulator) $nn,X Zero Page Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STA $" << hex << static_cast<uint16_t> (operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.x);
        cb.cpudatabus = cr.a;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x96:
        // Opcode 96 - STX (STore X index register) $nn,Y Zero Page Indexed with Y
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STX $" << hex << static_cast<uint16_t> (operand2) << ",Y" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.y);
        cb.cpudatabus = cr.x;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0x98:
        // Opcode 98 - TYA (Transfer Y index register to Accumulator)
#if _DEBUG
        cout << "OP: TYA" << '\t';
#endif // _DEBUG
        cr.a = cr.y;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0x9A:
        // Opcode 9A - TXS (Transfer X index register to Stack pointer)
#if _DEBUG
        cout << "OP: TXS" << '\t';
#endif // _DEBUG
        cr.sp = cr.x;
        cr.pc++;
        break;
    case 0x9D:
        // Opcode 9D - STA (STore Accumulator) $nnnn,x Absolute Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: STA $" << static_cast<uint16_t> ((operand3 << 8) | operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (((operand3 << 8) | operand2) + cr.x);
        cb.cpudatabus = cr.a;
        mem_cpuwrite(cb);
        cr.pc++;
        break;
    case 0xA0:
        // Opcode A0 - LDY (LoaD Y index register) #$nn Immediate
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDY #$" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cr.y = operand2;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA2:
        // Opcode A2 - LDX (LoaD X index register) #$nn Immediate
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDX #$" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cr.x = operand2;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA4:
        // Opcode A4 - LDY (LoaD X index register) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDY $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        mem_cpuread(cb);
        cr.y = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA5:
        // Opcode A5 - LDA (LoaD Accumulator) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA6:
        // Opcode A6 - LDX (LoaD X index register) $nn Zero Page
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDX $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = operand2;
        mem_cpuread(cb);
        cr.x = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA8:
        // Opcode A8 - TAY (Transfer Accumulator to Y index register)
#if _DEBUG
        cout << "OP: TAY" << '\t';
#endif // _DEBUG
        cr.y = cr.a;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xA9:
        // Opcode A9 - LDA (LoaD Accumulator) #$nn Immediate
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA #$" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        cr.a = operand2;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xAA:
        // Opcode AA - TAX (Transfer Accumulator to X index register)
#if _DEBUG
        cout << "OP: TAX" << '\t';
#endif // _DEBUG
        cr.x = cr.a;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xAC:
        // Opcode AC - LDY (LoaD Y index register) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDY $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        mem_cpuread(cb);
        cr.y = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xAD:
        // Opcode AD - LDA (LoaD Accumulator) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xAE:
        // Opcode AE - LDX (LoaD X index register) $nnnn Absolute
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDX $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> ((operand3 << 8) | operand2);
        mem_cpuread(cb);
        cr.x = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xB0:
        // Opcode B0 - BCS (Branch if Carry Set) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BCS $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.c == true) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0xB4:
        // Opcode B4 - LDY (LoaD Y index register) $nn,X Zero Page Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDY $" << hex << static_cast<uint16_t> (operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.x);
        mem_cpuread(cb);
        cr.y = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xB5:
        // Opcode B5 - LDA (LoaD Accumulator) $nn,X Zero Page Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA $" << hex << static_cast<uint16_t> (operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.x);
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xB6:
        // Opcode B6 - LDX (LoaD X index register) $nn,Y Zero Page Indexed with Y
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDX $" << hex << static_cast<uint16_t> (operand2) << ",Y" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint8_t> (operand2 + cr.y);
        mem_cpuread(cb);
        cr.x = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xB8:
        // Opcode B8 - CLV (CLear oVerflow flag)
#if _DEBUG
        cout << "OP: CLV" << '\t';
#endif // _DEBUG
        cr.v = false;
        cr.pc++;
        break;
    case 0xB9:
        // Opcode B9 - (LoaD Accumulator) $nnnn,Y Absolute Indexed with Y
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << ",Y" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (((operand3 << 8) | operand2) + cr.y);
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xBA:
        // Opcode BA - TSX (Transfer Stack pointer to X index register)
#if _DEBUG
        cout << "OP: TSX" << '\t';
#endif // _DEBUG
        cr.x = cr.sp;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xBC:
        // Opcode BC - LDY (LoaD Y index register) $nnnn,X Absolute Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDY $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (((operand3 << 8) | operand2) + cr.x);
        mem_cpuread(cb);
        cr.y = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xBD:
        // Opcode BD - LDA (LoaD Accumulator) $nnnn,X Absolute Indexed with X
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cpu_fetchnext(cb, cr);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDA $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << ",X" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (((operand3 << 8) | operand2) + cr.x);
        mem_cpuread(cb);
        cr.a = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.a;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xBE:
        // Opcode BE - LDX (LoaD X index register) $nnnn,Y Absolute Indexed with Y
        cpu_fetchnext(cb, cr);
        operand2 = cb.cpudatabus;
        cr.pc++;
        cb.cpuaddrbus = cr.pc;
        mem_cpuread(cb);
        operand3 = cb.cpudatabus;
#if _DEBUG
        cout << "OP: LDX $" << hex << static_cast<uint16_t> ((operand3 << 8) | operand2) << ",Y" << '\t';
#endif // _DEBUG
        cb.cpuaddrbus = static_cast<uint16_t> (((operand3 << 8) | operand2) + cr.y);
        mem_cpuread(cb);
        cr.x = cb.cpudatabus;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xC8:
        // Opcode C8 - INY (INcrement Y index register)
#if _DEBUG
        cout << "OP: INY" << '\t';
#endif // _DEBUG
        cr.y++;
        updaten = true;
        updatez = true;
        updateinput = cr.y;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xCA:
        // Opcode CA - DEX (CEcrement X index register)
#if _DEBUG
        cout << "OP: DEX" << '\t';
#endif // _DEBUG
        cr.x--;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xD0:
        // Opcode D0 - BNE (Branch if Not Equal) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BNE $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.z == false) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0xD8:
        // Opcode D8 - CLD (CLear Decimal flag)
#if _DEBUG
        cout << "OP: CLD" << '\t';
#endif // _DEBUG
        cr.d = false;
        cr.pc++;
        break;
    case 0xE8:
        // Opcode E8 - INX (INcrement X index register)
#if _DEBUG
        cout << "OP: INX" << '\t';
#endif // _DEBUG
        cr.x++;
        updaten = true;
        updatez = true;
        updateinput = cr.x;
        cpu_updateflags(updaten, updatez, cr, updateinput);
        cr.pc++;
        break;
    case 0xEA:
        // Opcode EA - NOP (No OPeration)
#if _DEBUG
        cout << "OP: NOP" << '\t';
#endif // _DEBUG
        cr.pc++;
        break;
    case 0xF0:
        // Opcode F0 - BEQ (Branch if EQual) $nn Relative
        cpu_fetchnext(cb, cr);
        reloperand = static_cast<int8_t> (cb.cpudatabus);
#if _DEBUG
        operand2 = cb.cpudatabus;
        cout << "OP: BEQ $" << hex << static_cast<uint16_t> (operand2) << '\t';
#endif // _DEBUG
        if (cr.z == true) {
            cr.pc = static_cast<uint16_t> (cr.pc + reloperand);
            cr.pc++;
            break;
        } else {
            cr.pc++;
            break;
        }
    case 0xF8:
        // Opcode F8 - SED (SEt Decimal flag)
#if _DEBUG
        cout << "OP: SED" << '\t';
#endif // _DEBUG
        cerr << "WARN: SED operation detected.  The 2A03 does not support 6502 decimal mode.\n";
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
        cerr << "ERROR: CPU opcode at 0x" << hex << cb.cpuaddrbus << " does not exist: " << hex << static_cast<uint16_t>(opcode) << '\n';
        canrun = false;
        return 1;
    default:
// TODO (chris#2#): More CPU opcodes, remove giant case case case block above
        cerr << "ERROR: CPU opcode at 0x" << hex << cb.cpuaddrbus << " not yet implemented: " << hex << static_cast<uint16_t> (opcode) << '\n';
        canrun = false;
        return 1;
    }

#if _DEBUG
        cout << "\tA: " << hex << static_cast<uint16_t> (cr.a) << \
        "\tX: " << hex << static_cast<uint16_t> (cr.x) << \
        "\tY: " << hex << static_cast<uint16_t> (cr.y) << \
        "\tSP: " << hex << static_cast<uint16_t> (cr.sp) << \
        "\tNVUBDIZC: " << cr.n << cr.v << cr.u << cr.b << cr.d << cr.i << cr.z << cr.c << '\n';
#endif // _DEBUG

    return 0;
}
