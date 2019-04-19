#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdint.h>

extern struct cpubus {
    uint16_t cpuaddrbus = 0x0000;
    uint8_t cpudatabus = 0x00;
} cpubus_t;

extern struct cpuregs {
    // Accumulator
    uint8_t a = 0x00;
    // X index register
    uint8_t x = 0x00;
    // Y index register
    uint8_t y = 0x00;
    // Stack Pointer
    uint8_t sp = 0xFD;
    // Program Counter
    uint16_t pc = 0x0000;

    // Status Flags
    // Carry
    bool c = false;
    // Zero
    bool z = false;
    // Interrupt Disable
    bool i = true;
    // Decimal (unused)
    bool d = false;
    // Breakpoint (unused)
    bool b = true;
    // Unused
    bool u = true;
    // Overflow
    bool v = false;
    // Negative
    bool n = false;
} cpuregs_t;

int cpu_init(cpubus &cb, cpuregs &cr);
int cpu_run(cpubus &cb, cpuregs &cr);

#endif // CPU_H_INCLUDED
