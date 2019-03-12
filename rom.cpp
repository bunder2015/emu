#include <iostream>     // for std::cout
#include <fstream>      // for std::ifstream
#include <cstring>      // for memcmp

#include "rom.h"        // for headermagic

using std::cout;
using std::ifstream;

int rom_ingest(char* romfile, char** rombuffer) {
    ifstream rf;                // Handle for ROM file opening
    unsigned long filesize;     // Size of ROM file

    rf.open(romfile, ifstream::binary);     // Open file

    if (rf.good()) {
        rf.seekg(0, ifstream::end);         // Go to end of file
        filesize = rf.tellg();              // Read size of ROM file
        rf.seekg(0, ifstream::beg);         // Go back to beginning of file
        *rombuffer = new char[filesize];    // rombuffer holds the file while we read the header and prepare the memory map
        rf.read(*rombuffer, filesize);      // Read file to rombuffer
        rf.close();                         // Close file
        return 0;
    } else {
        cout << "ERROR: File could not be read!\n";
        return 1;
    }
}

int rom_headerparse(char** rombuffer, int* prgromsize, int* chrromsize,
                    int* mirrormode, int* prgrampresence, int* fourscreenmode,
                    int* prgramsize, int* tvsystem) {

    int inesformat = 0;
// TODO (chris#9#): consider adding "archiac" iNES ROM support
    if (memcmp(*rombuffer, headermagic, sizeof(headermagic)) == 0) {    // Compare header magic bytes
        if ((*(*rombuffer + 0x07) & 0x0C) == 0x08) {            // If byte 7, bits 8 and 4, are (1,0)
            inesformat = 2;                                     // we are iNES format 2.0
        } else if (((*(*rombuffer + 0x07) & 0x0C) == 0x00)      // If byte 7, bits 8 and 4, are (0,0)
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

            // Stop right now if we see something we don't like
            if ((*(*rombuffer + 0x07) & 0x01) == 0x01) {            // If byte 7 bit 1 is 1
                cout << "ERROR: VS ROMs are not supported\n";       // this is a VS System ROM
                return 1;
            } else if ((*(*rombuffer + 0x07) & 0x02) == 0x02) {     // If byte 7 bit 2 is 1
                cout << "ERROR: PC10 ROMs are not supported\n";     // this is a PlayChoice 10 ROM
                return 1;
            } else if ((*(*rombuffer + 0x06) & 0x04) == 0x04) {     // If byte 6, bit 4 is 1
                cout << "ERROR: Trainers are not supported\n";      // we have a trainer which unmodified ROMs do not have
                return 1;
            } else if (((*(*rombuffer + 0x09) & 0x02) == 0x02)      // If byte 9, bits 2 through 128, are 1
                       || ((*(*rombuffer + 0x09) & 0x04) == 0x04)   // then file might be corrupted
                       || ((*(*rombuffer + 0x09) & 0x08) == 0x08)   // or empty spaces in the header
                       || ((*(*rombuffer + 0x09) & 0x08) == 0x08)   // are used incorrectly
                       || ((*(*rombuffer + 0x09) & 0x10) == 0x10)
                       || ((*(*rombuffer + 0x09) & 0x20) == 0x20)
                       || ((*(*rombuffer + 0x09) & 0x40) == 0x40)
                       || ((*(*rombuffer + 0x09) & 0x80) == 0x80)) {
                cout << "ERROR: Byte 9 reserved bits are not zero\n";
                return 1;
            }

            *prgromsize = (*(*rombuffer + 0x04) & 0xFF);        // PRG ROM is byte 4 * 16k in size
            cout << "INFO: PRG ROM size: " << *prgromsize * 16384 << " bytes total\n";

            if ((*(*rombuffer + 0x05) & 0xFF) > 0) {
                *chrromsize = (*(*rombuffer + 0x05) & 0xFF);    // CHR ROM is byte 5 * 8k in size
                cout << "INFO: CHR ROM size: " << *chrromsize * 8192 << " bytes total\n";
            } else {
                cout << "INFO: No CHR ROM present\n";
                /* TODO (chris#3#): Upon analysis of various ROMs it would appear that
                *  if they contain CHR RAM instead of a CHR ROM, the game has 8kb
                *  for this purpose
                */
                //return 1;
            }

            if ((*(*rombuffer + 0x06) & 0x02) == 0x02) {        // If byte 6, bit 2 is 1
                *prgrampresence = 1;                            // we have a PRG RAM
                cout << "INFO: PRG RAM present\n";
                /* FIXME (chris#8#): Upon analysis of various ROMs it would appear that
                *  all of them lie about their PRG RAM size.  All games that have PRG RAM
                *  have 8kb with the exception of:
                *   Gauntlet: 2kb
                *   RacerMate Challenge II (unlicensed): 2 * 32kb
                *   Romance of the Three Kingdoms II: 32kb
                */
                *prgramsize = (*(*rombuffer + 0x08) & 0xFF);            // PRG RAM is byte 8 * 8kb in size
                if ((*prgrampresence == 1) && (*prgramsize == 0)) {     // If we have PRG RAM but don't specify size
                    *prgramsize = 1;                                    // fudge the numbers, see fixme above
                }
                cout << "INFO: PRG RAM size: " << *prgramsize * 8192 << " bytes total\n";
            } else {
                cout << "INFO: No PRG RAM present\n";
            }

            if ((*(*rombuffer + 0x06) & 0x08) == 0x08) {        // If byte 6, bit 8 is 1
                *fourscreenmode = 1;                            // we are four screen mirroring (Gauntlet, Rad Racer II)
                if ((*(*rombuffer + 0x06) & 0x01) == 0x01) {    // If byte 6, bit 1 is 1
                    *mirrormode = 1;                            // this bit usually does nothing when in four screen
                    cout << "INFO: Four screen mirroring (vertical bit also present)\n";
                } else {
                    cout << "INFO: Four screen mirroring (vertical bit not present)\n";
                }
// TODO (chris#1#): Check for mappers before checking V/H mirroring as the mapper can override mirror bits in header
            } else if ((*(*rombuffer + 0x06) & 0x01) == 0x01) { // If byte 6, bit 1 is 1
                *mirrormode = 1;                                // we are vertical mirroring
                cout << "INFO: Vertical mirroring\n";
            } else {
                cout << "INFO: Horizontal mirroring\n";
            }

            if ((*(*rombuffer + 0x09) & 0x01) == 0x01) {        // If byte 9 bit 1 is 1
                *tvsystem = 1;                                  // we are PAL
                cout << "INFO: TV system: PAL\n";               // NESdev claims this is spec "but nobody uses it"
            } else {
                cout << "INFO: TV system: NTSC\n";
            }
// TODO (chris#1#): More iNES v1 more header elements - mapper bits, byte 10, unused(?) byte 11
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
