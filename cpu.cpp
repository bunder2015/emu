#include "cpu.h"    // for cpubus
#include "mmu.h"    // for cpumem_read cpumem_write

cpuregs cr;

int cpu_init(cpubus &cb) {
    // Set the Program Counter
    cb.cpuaddrbus = 0xFFFC;
    cpumem_read(cb);
    uint8_t pclow = cb.cpudatabus;
    cb.cpuaddrbus = 0xFFFD;
    cpumem_read(cb);
    uint8_t pchigh = cb.cpudatabus;
    cr.pc = static_cast <uint16_t> (pchigh << 8 | pclow);

// FIXME (chris#6#): Remove CPU testing code
    cb.cpuaddrbus = 0x8002;
    cpumem_read(cb);
    cb.cpuaddrbus = 0x0000;
    cpumem_write(cb);

    return 0;
}
