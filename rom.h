#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

#include "mmu.h"

const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A
const char headerempty[4] = { 0x00, 0x00, 0x00, 0x00 }; // Four empty bytes

int rom_ingest(char *romfile, romheader &rh);
int rom_headerparse(romheader &rh);

#endif // ROM_H_INCLUDED
