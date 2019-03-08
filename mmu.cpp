#include <iostream>     // for std::cout
#include <fstream>      // for std::ifstream
#include <cstring>      // for memcmp

using std::cout;
using std::ifstream;

char* rombuffer;        // in memory copy of the entire rom file

int rom_ingest(char* romfile) {
    ifstream rf;
    unsigned long filesize;

    rf.open(romfile, ifstream::binary);     // open file

    if (rf.good()) {
        rf.seekg(0, ifstream::end);         // go to end of file
        filesize = rf.tellg();              // read length
        rf.seekg(0, ifstream::beg);         // go back to beginning of file
        rombuffer = new char[filesize];     // rombuffer holds the file while we read the header and prepare the memory map
        rf.read(rombuffer, filesize);       // read file to rombuffer
        rf.close();                         // close file
        return 0;
    } else {
        cout << "ERROR: File could not be read!\n";
        return 1;
    }
}

int rom_headerparse() {
// TODO (chris#9#): consider adding pre iNES ROM support
    static const char headermagic[4] = { 0x4E, 0x45, 0x53, 0x1A };  // "N", "E", "S", 0x1A

    if (memcmp(rombuffer, headermagic, sizeof(headermagic)) == 0) { // compare header magic bytes
// TODO (chris#1#): verify iNES v1 vs v2, break down header elements to choose memory mapper and prepare CPU/PPU memory map
        return 0;
    } else {
        cout << "ERROR: Not an iNES file! (missing magic)\n";
        return 1;
    }
}

int mmu_init(char* romfile) {
    if (rom_ingest(romfile) == 0) {
        if (rom_headerparse() == 0) {
            // something after headerparse
            return 0;
        } else {
            // rom_headerparse failed
            return 1;
        }
    } else {
        // rom_ingest failed
        return 1;
    }
}
