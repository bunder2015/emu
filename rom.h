#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A
const char headerempty[4] = { 0x00, 0x00, 0x00, 0x00 }; // Four empty bytes

int rom_ingest(char* romfile, char** rombuffer);
int rom_headerparse(char** rombuffer, int* prgromsize, int* prgramsize,
                    int* chrromsize, int *chrramsize, int* mapper,
                    int* mirrormode, int* batterypresent, int* fourscreenmode,
                    int* tvsystem, int* busconflicts);

#endif // ROM_H_INCLUDED
