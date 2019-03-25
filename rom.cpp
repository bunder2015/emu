#include <cstring>      // for memcmp
#include <fstream>      // for std::ifstream
#include <iostream>     // for std::cout std::cerr

#include "rom.h"        // for headermagic headerempty

using std::cerr;
using std::cout;
using std::ifstream;

unsigned long filesize = 0;

int rom_ingest(char *romfile, romheader &rh) {
    ifstream rf;                            // Handle for ROM file opening

    rf.open(romfile, ifstream::binary);     // Open file

    if (rf.good()) {
        rf.seekg(0, ifstream::end);         // Go to end of file
        filesize = static_cast <unsigned long> (rf.tellg());    // Read size of ROM file
        rf.seekg(0, ifstream::beg);         // Go back to beginning of file
        rh.rombuffer = new char[filesize];  // rombuffer holds the file while we read the header and prepare the memory map
// TODO (chris#1#): warning: conversion to ‘std::streamsize {aka long int}’ from ‘long unsigned int’ may change the sign of the result [-Wsign-conversion]
        rf.read(rh.rombuffer, filesize);    // Read file to rombuffer
        rf.close();                         // Close file
        return 0;
    } else {
        cerr << "ERROR: File could not be read!\n";
        return 1;
    }
}

int rom_headerparse(romheader &rh) {
    if (memcmp(rh.rombuffer, headermagic, sizeof(headermagic)) == 0) {  // Compare header magic bytes
        unsigned short inesformat = 0;

        if ((*(rh.rombuffer + 7) & 0x0C) == 0x08) {         // If byte 7, bits 8 and 4, are (1,0)
            inesformat = 2;                                 // we are iNES format 2.0
        } else if (((*(rh.rombuffer + 7) & 0x0C) == 0x00)   // If byte 7, bits 8 and 4, are (0,0)
                   && (memcmp((rh.rombuffer + 12), headerempty, sizeof(headerempty)) == 0)) { // and bytes 12 through 15 are 0
            inesformat = 1;                                 // we are iNES format 0.7 or 1.0
        }

        if (inesformat == 1) {
            cout << "INFO: iNES header format 1\n";

            // Stop right now if we see something we don't like
            if ((*(rh.rombuffer + 7) & 0x01) == 0x01) {             // If byte 7 bit 1 is 1
                cerr << "ERROR: VS ROMs are not supported\n";       // this is a VS System ROM
                return 1;
            } else if ((*(rh.rombuffer + 7) & 0x02) == 0x02) {      // If byte 7 bit 2 is 1
                cerr << "ERROR: PC10 ROMs are not supported\n";     // this is a PlayChoice 10 ROM
                return 1;
            } else if ((*(rh.rombuffer + 6) & 0x04) == 0x04) {      // If byte 6, bit 4 is 1
                cerr << "ERROR: Trainers are not supported\n";      // we have a trainer which unmodified ROMs do not have
                return 1;
            } else if ((*(rh.rombuffer + 9) & 0xFE) > 0) {          // If byte 9, bits 2 through 128, are not 0
                cerr << "ERROR: Reserved header byte 9 bits are not zero\n";
                return 1;
            } else if ((*(rh.rombuffer + 10) & 0xCC) > 0) {         // If byte 10, bits 128,64,8,4 are not 0
                cerr << "ERROR: Unused header byte 10 bits are not zero\n";
                return 1;
            } else if ((*(rh.rombuffer + 11) & 0xFF) > 0) {         // If byte 11 is not 0
                cerr << "ERROR: Unused header byte 11 is not zero\n";
                return 1;
            }

            unsigned short mapperlow = ((*(rh.rombuffer + 6) & 0xF0) >> 4);    // Lower nibble is the upper nibble of byte 6
            unsigned short mapperhigh = ((*(rh.rombuffer + 7) & 0xF0) >> 4);   // Upper nibble is the upper nibble of byte 7
            mapperhigh = static_cast <unsigned short> (mapperhigh << 4);       // Move mapperhigh back to the upper nibble
            rh.mapper = static_cast <unsigned short> (mapperlow + mapperhigh); // Combine both nibbles into a byte
            cout << "INFO: Mapper number " << rh.mapper << '\n';

            rh.prgromsize = static_cast <unsigned long> ((*(rh.rombuffer + 4) & 0xFF) * 16384); // PRG ROM is byte 4 * 16k in size
            cout << "INFO: PRG ROM size: " << rh.prgromsize << " bytes total\n";

            if ((*(rh.rombuffer + 5) & 0xFF) > 0) {
                rh.chrromsize = static_cast <unsigned long> ((*(rh.rombuffer + 5) & 0xFF) * 8192);  // CHR ROM is byte 5 * 8k in size
                cout << "INFO: CHR ROM size: " << rh.chrromsize << " bytes total\n";
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
                rh.chrramsize = 8192;
                cout << "INFO: CHR RAM size: " << rh.chrramsize << " bytes total\n";
            }

            unsigned long romsize = rh.prgromsize + rh.chrromsize;  // ROM file should be as big as the header and its constituent ROMs
            if ((romsize + 16) != filesize) {                       // we have no way of knowing whether the boundary between ROMs is correct
                cerr << "ERROR: File size (" << filesize << " bytes) does not match reported ROM size (" << romsize << " bytes + 16 byte header)\n";
                return 1;
            }

// TODO (chris#8#): Make this AND instead of OR for strictness
            if (((*(rh.rombuffer + 6) & 0x02) == 0x02)              // If byte 6, bit 2 is 1
                    || ((*(rh.rombuffer + 10) & 0x10) == 0x10)) {   // or byte 10, bit 16 is 1
                rh.batterypresent = true;                           // we have a battery
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
            rh.prgramsize = static_cast <unsigned long> ((*(rh.rombuffer + 8) & 0xFF) * 8192);  // PRG RAM is byte 8 * 8kb in size

            if (rh.prgramsize > 0) {
                cout << "INFO: PRG RAM size: " << rh.prgramsize << " bytes total\n";
            } else {
                if ((rh.batterypresent == true) && (rh.prgramsize == 0)) {          // Battery implies PRG RAM but you must specify it
                    cerr << "ERROR: Battery without PRG RAM, fix header byte 8\n";  // if ROMs didn't lie, the only two games to fail here are StarTropics
                    return 1;
                }

                cout << "INFO: No PRG RAM present\n";               // There is no way to know if we need a PRG RAM without battery bit
            }

            if ((*(rh.rombuffer + 6) & 0x08) == 0x08) {             // If byte 6, bit 8 is 1
                rh.fourscreenmode = true;                           // we are four screen mirroring (Gauntlet, Rad Racer II)
                cout << "INFO: Four screen mirroring\n";

                if ((*(rh.rombuffer + 6) & 0x01) == 0x01) {         // If byte 6, bit 1 is 1
                    rh.mirrormode = true;                           // this bit probably does nothing when in four screen
                }

            } else if ((*(rh.rombuffer + 6) & 0x01) == 0x01) {      // If byte 6, bit 1 is 1
                rh.mirrormode = true;                               // we are vertical mirroring
                cout << "INFO: Vertical mirroring\n";
            } else {
                cout << "INFO: Horizontal mirroring\n";
            }

// TODO (chris#8#): Make this AND instead of OR for strictness
            if (((*(rh.rombuffer + 9) & 0x01) == 0x01)              // If byte 9, bit 1 is 1
                    || ((*(rh.rombuffer + 10) & 0x02) == 0x02)) {   // or byte 10, bit 2 is 1
                rh.tvsystem = true;                                 // we are PAL
                cout << "INFO: TV system: PAL\n";
            } else {
                cout << "INFO: TV system: NTSC\n";
            }

            if ((*(rh.rombuffer + 10) & 0x20) == 0x20) {            // If byte 10, bit 32 is 1
                rh.busconflicts = true;                             // we have bus conflicts to be careful about
                cout << "INFO: Bus conflicts possible\n";           // if we decide to emulate them
            }

            return 0;
        } else if (inesformat == 2) {
// TODO (chris#5#): iNES v2 headers
            cout << "ERROR: iNES header format 2 not yet implemented\n";
            return 1;
        } else {
            cerr << "ERROR: Not an iNES 0.7/1.0 or 2.0 file! (valid magic, format corrupted)\n";
            return 1;
        }

    } else {
        cerr << "ERROR: Not an iNES file! (invalid magic)\n";
        return 1;
    }
}
