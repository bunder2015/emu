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

int rom_headerparse(char** rombuffer, int* prgromsize, int* chrromsize,
                    int* mirrormode, int* prgrampresence, int* fourscreenmode) {
    int inesformat = 0;
// TODO (chris#9#): consider adding "archiac" iNES ROM support
    if (memcmp(*rombuffer, headermagic, sizeof(headermagic)) == 0) {    // compare header magic bytes
        if ((*(*rombuffer + 0x07) & 0x0C) == 0x08) {            // if byte 7, bits 8 and 4, are (1,0)
            inesformat = 2;                                     // we are iNES format 2.0
        } else if (((*(*rombuffer + 0x07) & 0x0C) == 0x00)      // if byte 7, bits 8 and 4, are (0,0)
                   && ((*(*rombuffer + 0x0C) & 0xFF) == 0x00)   // and bytes 12-15 are 0
                   && ((*(*rombuffer + 0x0D) & 0xFF) == 0x00)
                   && ((*(*rombuffer + 0x0E) & 0xFF) == 0x00)
                   && ((*(*rombuffer + 0x0F) & 0xFF) == 0x00)) {
            inesformat = 1;                                     // we are iNES format 0.7 or 1.0
        }
// TODO (chris#1#): break down header elements to choose memory mapper and prepare CPU/PPU memory map
        if (inesformat == 2) {
            cout << "INFO: iNES format 2\n";
// TODO (chris#4#): iNES v2 headers
            return 0;
        } else if (inesformat == 1) {
            cout << "INFO: iNES format 1\n";

            *prgromsize = (*(*rombuffer + 0x04) & 0xFF);        // PRG ROM is byte 4 * 16k in size
            cout << "INFO: PRG ROM size: " << *prgromsize * 16384 << " bytes total\n";

            if ((*(*rombuffer + 0x05) & 0xFF) > 0) {
                *chrromsize = (*(*rombuffer + 0x05) & 0xFF);    // CHR ROM is byte 5 * 8k in size
                cout << "INFO: CHR ROM size: " << *chrromsize * 8192 << " bytes total\n";
            } else {
                cout << "INFO: No CHR ROM, CHR RAM instead\n";
// TODO (chris#2#): CHR RAM
            }

            if ((*(*rombuffer + 0x06) & 0x01) == 0x01) {        // if byte 6, bit 1 is 1
                *mirrormode = 1;                                // we are vertical mirroring
                cout << "INFO: Vertical mirroring\n";
            } else {
                cout << "INFO: Horizontal mirroring\n";
            }

            if ((*(*rombuffer + 0x06) & 0x02) == 0x02) {        // if byte 6 bit 2 is 1
                *prgrampresence = 1;                            // we have a PRG RAM
                cout << "INFO: PRG RAM present\n";
            } else {
                cout << "INFO: No PRG RAM present\n";
            }

            if ((*(*rombuffer + 0x06) & 0x04) == 0x04) {        // if byte 6 bit 4 is 1
                cout << "ERROR: We do not support trainers\n";  // we have a trainer which unmodified ROMs do not have
                return 1;
            }

            if ((*(*rombuffer + 0x06) & 0x08) == 0x08) {        // if byte 6 bit 8 is 1
                *fourscreenmode = 1;                            // we are actually four screen mirroring
                cout << "INFO: Four screen mirroring\n";
            }
// TODO (chris#1#): more iNES v1 more header elements

            return 0;
        } else {
            cout << "ERROR: Not an iNES 0.7/1.0 or 2.0 file! (valid magic, format trashed)\n";
            return 1;
        }
    } else {
        cout << "ERROR: Not an iNES file! (invalid magic)\n";
        return 1;
    }
}
