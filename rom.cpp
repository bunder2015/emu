#include <iostream>     // for std::cout
#include <fstream>      // for std::ifstream
#include <cstring>      // for memcmp

#include "rom.h"        // for headermagic

using std::cout;
using std::ifstream;

int rom_ingest(char* romfile, char** rombuffer) {
    ifstream rf;                // handler for ROM file opening
    unsigned long filesize;     // size of ROM file

    rf.open(romfile, ifstream::binary);     // open file

    if (rf.good()) {
        rf.seekg(0, ifstream::end);         // go to end of file
        filesize = rf.tellg();              // read size of ROM file
        rf.seekg(0, ifstream::beg);         // go back to beginning of file
        *rombuffer = new char[filesize];    // rombuffer holds the file while we read the header and prepare the memory map
        rf.read(*rombuffer, filesize);      // read file to rombuffer
        rf.close();                         // close file
        return 0;
    } else {
        cout << "ERROR: File could not be read!\n";
        return 1;
    }
}

int rom_headerparse(char** rombuffer) {
    int inesformat = 0;
// TODO (chris#9#): consider adding "archiac" iNES ROM support
    if (memcmp(*rombuffer, headermagic, sizeof(headermagic)) == 0) {    // compare header magic bytes
// TODO (chris#1#): verify iNES v1 vs v2, break down header elements to choose memory mapper and prepare CPU/PPU memory map
// TODO (chris#1#): this doesn't work, yay pointers again
//        if (((**rombuffer + 7) & 0x0C) == 0x08) {   // if byte 7, bits 8 and 4, are (1,0)
//            inesformat = 2;                     // we are iNES format 2.0
//        } else if ((((*rombuffer + 7) & 0x0C) == 0)    // if byte 7, bits 8 and 4, are (0,0)
//                    && (((*rombuffer + 0x0C) & 0x00) == 0)   // and bytes 12-15 are 0
//                    && (((*rombuffer + 0x0D) & 0x00) == 0)
//                    && (((*rombuffer + 0x0E) & 0x00) == 0)
//                    && (((*rombuffer + 0x0F) & 0x00) == 0)) {
//            inesformat = 1;                     // we are iNES format 0.7 or 1.0
//        } else {
//            cout << "ERROR: Not an iNES 0.7/1.0 or 2.0 file!\n";
//            return 1;
//        }
        return 0;
    } else {
        cout << "ERROR: Not an iNES file! (missing magic)\n";
        return 1;
    }
}
