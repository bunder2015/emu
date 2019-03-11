#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A }; // "N", "E", "S", 0x1A

int rom_ingest(char* romfile, char** rombuffer);
int rom_headerparse(char** rombuffer, int* prgromsize, int* chrromsize,
                    int* mirrormode, int* prgrampresence);

#endif // ROM_H_INCLUDED
