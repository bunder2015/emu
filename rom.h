#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A
const char headerempty[4] = { 0x00, 0x00, 0x00, 0x00 }; // Four empty bytes

int rom_ingest(char* romfile, char** rombuffer);
int rom_headerparse(char** rombuffer,
                    unsigned short* mapper,
                    unsigned long* prgromsize,
                    unsigned long* prgramsize,
                    unsigned long* chrromsize,
                    unsigned long* chrramsize,
                    bool* batterypresent,
                    bool* mirrormode,
                    bool* fourscreenmode,
                    bool* tvsystem,
                    bool* busconflicts);

#endif // ROM_H_INCLUDED
