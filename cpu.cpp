#include <iostream> // for std::cerr std::cout std::hex

#include "cpu.h"    // for cpubus cpuregs
#include "mmu.h"    // for cpumem_read cpumem_write

using std::cerr;
using std::cout;
using std::hex;

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

    if (cr.pc <= 0x8000) {
        cerr << "ERROR: CPU boot PC is not in PRG ROM memory!\n";
        return 1;
    } else {
        cout << "INFO: CPU execution will begin at 0x" << hex << cr.pc << '\n';
        return 0;
    }
}
