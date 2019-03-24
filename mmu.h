#ifndef MMU_H_INCLUDED
#define MMU_H_INCLUDED

extern struct romheader {
    char *rombuffer;                // In memory copy of the entire ROM file
    unsigned short mapper = 0;      // iNES mapper number
    unsigned long prgromsize = 0;   // Size of cartridge PRG ROM
    unsigned long prgramsize = 0;   // Size of cartridge PRG RAM (WRAM + save WRAM)
    unsigned long chrromsize = 0;   // Size of cartridge CHR ROM
    unsigned long chrramsize = 0;   // Size of cartridge CHR RAM (VRAM)
    bool batterypresent = false;    // Cartridge PRG RAM save battery presence toggle
    bool mirrormode = false;        // Horizontal/Vertical mirroring toggle (true = vertical)
    bool fourscreenmode = false;    // Four screen mirroring toggle
    bool tvsystem = false;          // NTSC/PAL TV system toggle (true = PAL)
    bool busconflicts = false;      // Bus conflicts
}rh;


int mmu_init(char *romfile);

#endif // MMU_H_INCLUDED
