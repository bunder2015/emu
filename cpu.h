#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdint.h>

extern struct cpubus {
    uint16_t cpuaddrbus = 0x0000;
    uint8_t cpudatabus = 0x00;
} cpubus_t;

extern struct cpuregs {
    uint8_t a = 0x00;       // Accumulator
    uint8_t x = 0x00;       // X register
    uint8_t y = 0x00;       // Y register
    uint8_t sp = 0xFD;      // Stack Pointer
    uint16_t pc = 0x0000;   // Program Counter
    bool c = false;             // Carry
    bool z = false;             // Zero
    bool i = true;              // Interrput
    bool d = false;             // Decimal (unused)
    bool b = true;              // Breakpoint (unused)
    bool u = true;              // Unused
    bool v = false;             // Overflow
    bool n = false;             // Negative
} cpuregs_t;

int cpu_init(cpubus &cb);

#endif // CPU_H_INCLUDED
