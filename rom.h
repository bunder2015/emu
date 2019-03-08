#ifndef ROM_H_INCLUDED
#define ROM_H_INCLUDED

int rom_ingest(char* romfile, char* rombuffer);
int rom_headerparse(char* rombuffer);

#endif // ROM_H_INCLUDED
