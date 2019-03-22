#include <iostream>     // for std::cout std::cerr
#include <fstream>      // for std::ifstream
#include <cstring>      // for memcmp

#include "rom.h"        // for headermagic

using std::cout;
using std::cerr;
using std::ifstream;

int rom_ingest(char *romfile, char **rombuffer) {
    ifstream rf;                                // Handle for ROM file opening

    rf.open(romfile, ifstream::binary);         // Open file

    if (rf.good()) {
        rf.seekg(0, ifstream::end);             // Go to end of file
        unsigned long filesize = rf.tellg();    // Read size of ROM file
        rf.seekg(0, ifstream::beg);             // Go back to beginning of file
        *rombuffer = new char[filesize];        // rombuffer holds the file while we read the header and prepare the memory map
        rf.read(*rombuffer, filesize);          // Read file to rombuffer
        rf.close();                             // Close file
        return 0;
    } else {
        cerr << "ERROR: File could not be read!\n";
        return 1;
    }
}

int rom_headerparse(char **rombuffer,
                    unsigned short *mapper,
                    unsigned long *prgromsize,
                    unsigned long *prgramsize,
                    unsigned long *chrromsize,
                    unsigned long *chrramsize,
                    bool *batterypresent,
                    bool *mirrormode,
                    bool *fourscreenmode,
                    bool *tvsystem,
                    bool *busconflicts) {

    unsigned short inesformat = 0;
    unsigned short mapperlow = 0;
    unsigned short mapperhigh = 0;

    if (memcmp(*rombuffer, headermagic, sizeof(headermagic)) == 0) {    // Compare header magic bytes
        if ((*(*rombuffer + 0x07) & 0x0C) == 0x08) {                // If byte 7, bits 8 and 4, are (1,0)
            inesformat = 2;                                         // we are iNES format 2.0
        } else if (((*(*rombuffer + 0x07) & 0x0C) == 0x00)          // If byte 7, bits 8 and 4, are (0,0)
                   && (memcmp(*rombuffer + 0x0C, headerempty, sizeof(headerempty)) == 0)) { // and bytes 12 through 15 are 0
            inesformat = 1;                                         // we are iNES format 0.7 or 1.0
        }

        if (inesformat == 1) {
            cout << "INFO: iNES header format 1\n";

            // Stop right now if we see something we don't like
            if ((*(*rombuffer + 0x07) & 0x01) == 0x01) {            // If byte 7 bit 1 is 1
                cerr << "ERROR: VS ROMs are not supported\n";       // this is a VS System ROM
                return 1;
            } else if ((*(*rombuffer + 0x07) & 0x02) == 0x02) {     // If byte 7 bit 2 is 1
                cerr << "ERROR: PC10 ROMs are not supported\n";     // this is a PlayChoice 10 ROM
                return 1;
            } else if ((*(*rombuffer + 0x06) & 0x04) == 0x04) {     // If byte 6, bit 4 is 1
                cerr << "ERROR: Trainers are not supported\n";      // we have a trainer which unmodified ROMs do not have
                return 1;
            } else if ((*(*rombuffer + 0x09) & 0xFE) > 0) {         // If byte 9, bits 2 through 128, are not 0
                cerr << "ERROR: Reserved header byte 9 bits are not zero\n";
                return 1;
            } else if ((*(*rombuffer + 0x0A) & 0xCC) > 0) {         // If byte 10, bits 128,64,8,4 are not 0
                cerr << "ERROR: Unused header byte 10 bits are not zero\n";
                return 1;
            } else if ((*(*rombuffer + 0x0B) & 0xFF) > 0) {         // If byte 11 is not 0
                cerr << "ERROR: Unused header byte 11 is not zero\n";
                return 1;
            }

            mapperlow = ((*(*rombuffer + 0x06) & 0xF0) >> 4);       // Lower nibble is the upper nibble of byte 6
            mapperhigh = ((*(*rombuffer + 0x07) & 0xF0) >> 4);      // Upper nibble is the upper nibble of byte 7
            mapperhigh = mapperhigh << 4;                           // Move mapperhigh back to the upper nibble
            *mapper = mapperlow + mapperhigh;                       // Combine both nibbles into a byte
            cout << "INFO: Mapper number " << *mapper << "\n";

            *prgromsize = ((*(*rombuffer + 0x04) & 0xFF) * 16384);  // PRG ROM is byte 4 * 16k in size
            cout << "INFO: PRG ROM size: " << *prgromsize << " bytes total\n";

            if ((*(*rombuffer + 0x05) & 0xFF) > 0) {
                *chrromsize = ((*(*rombuffer + 0x05) & 0xFF) * 8192);   // CHR ROM is byte 5 * 8k in size
                cout << "INFO: CHR ROM size: " << *chrromsize << " bytes total\n";
            } else {
                /* TODO (chris#9#): The iNES v1 header does not specify CHR RAM size
                *  Upon analysis of various ROMs it would appear that if
                *  they contain CHR RAM instead of a CHR ROM, the game has 8kb
                *  for this purpose with the exception of:
                *   Gauntlet: 2kb
                *   Rad Racer: 4kb (it has 8 on the board but only uses 4?)
                *   Videomation: 2 * 8kb
                *   RacerMate Challenge II (unlicensed): 2 * 32kb
                */
                *chrramsize = 8192;
                cout << "INFO: CHR RAM size: " << *chrramsize << " bytes total\n";
            }

// TODO (chris#8#): Make this AND instead of OR for strictness
            if (((*(*rombuffer + 0x06) & 0x02) == 0x02)             // If byte 6, bit 2 is 1
                    || ((*(*rombuffer + 0x0A) & 0x10) == 0x10)) {   // or byte 10, bit 16 is 1
                *batterypresent = true;                             // we have a battery
                cout << "INFO: Battery backed PRG RAM present\n";
            }

            /* TODO (chris#9#): Fix bad ROMs
            *  Upon analysis of various ROMs it would appear that all of them lie
            *  about their PRG RAM size.  All games that have battery backed PRG RAM
            *  have 8kb with the exception of:
            *   StarTropics: Battery but no PRG RAM, 1k on MMC6 but not included in header
            *   Zoda's Revenge: StarTropics II: Battery but no PRG RAM, 1k on MMC6 but not included in the header
            *   Bandit Kings of Ancient China: 2 * 8kb, 8kb on battery
            *   Genghis Khan: 2 * 8kb, 8kb on battery
            *   L'Empereur: 2 * 8kb, 8kb on battery
            *   Nobunaga's Ambition: 2 * 8kb, 8kb on battery
            *   Nobunaga's Ambition II: 2 * 8kb, 8kb on battery
            *   Romance of the Three Kingdoms: 2 * 8kb, 8kb on battery
            *   Uncharted Waters: 2 * 8kb, 8kb on battery
            *   Romance of the Three Kingdoms II: 32kb
            *  On second thought, we could always just fix the ROMs (except for StarTropics)
            */
            *prgramsize = ((*(*rombuffer + 0x08) & 0xFF) * 8192);   // PRG RAM is byte 8 * 8kb in size
            if (*prgramsize > 0) {
                cout << "INFO: PRG RAM size: " << *prgramsize << " bytes total\n";
            } else {
                if ((*batterypresent == true) && (*prgramsize == 0)) { // Battery implies PRG RAM but you must specify it
                    cerr << "ERROR: Battery without PRG RAM, fix header byte 8\n";  // if ROMs didn't lie, the only two games to fail here are StarTropics
                    return 1;
                }
                cout << "INFO: No PRG RAM present\n";               // There is no way to know if we need a PRG RAM without battery bit
            }

            if ((*(*rombuffer + 0x06) & 0x08) == 0x08) {            // If byte 6, bit 8 is 1
                *fourscreenmode = true;                             // we are four screen mirroring (Gauntlet, Rad Racer II)
                cout << "INFO: Four screen mirroring\n";
                if ((*(*rombuffer + 0x06) & 0x01) == 0x01) {        // If byte 6, bit 1 is 1
                    *mirrormode = true;                             // this bit probably does nothing when in four screen
                }
            } else if ((*(*rombuffer + 0x06) & 0x01) == 0x01) {     // If byte 6, bit 1 is 1
                *mirrormode = true;                                 // we are vertical mirroring
                cout << "INFO: Vertical mirroring\n";
            } else {
                cout << "INFO: Horizontal mirroring\n";
            }
// TODO (chris#8#): Make this AND instead of OR for strictness
            if (((*(*rombuffer + 0x09) & 0x01) == 0x01)             // If byte 9, bit 1 is 1
                    || ((*(*rombuffer + 0x0A) & 0x02) == 0x02)) {   // or byte 10, bit 2 is 1
                *tvsystem = true;                                   // we are PAL
                cout << "INFO: TV system: PAL\n";
            } else {
                cout << "INFO: TV system: NTSC\n";
            }

            if ((*(*rombuffer + 0x0A) & 0x20) == 0x20) {            // If byte 10, bit 32 is 1
                *busconflicts = true;                               // we have bus conflicts to be careful about
                cout << "INFO: Bus conflicts possible\n";           // if we decide to emulate them
            }

            return 0;
        } else if (inesformat == 2) {
            cout << "INFO: iNES header format 2\n";
// TODO (chris#5#): iNES v2 headers
            return 0;
        } else {
            cerr << "ERROR: Not an iNES 0.7/1.0 or 2.0 file! (valid magic, format corrupted)\n";
            return 1;
        }
    } else {
        cerr << "ERROR: Not an iNES file! (invalid magic)\n";
        return 1;
    }
}
