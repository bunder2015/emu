# emu

Nothing to see here, move along... Yes, I know there are a million NES
emulators out there. It's a learning project and i don't plan on releasing it.

# A note about ROM support and header strictness

Emu is being written to support NTSC-U (ie: NES) games, NTSC-J (Famicom) and
PAL support may come later, however NTSC-J will most likely work.  VS/PC10 is
not supported.  Trainers are not supported.

At this time only mapper 0 is supported.

The iNES format has been around since the early-to-mid 1990s, and many ROMs are
wrong. This emulator is being written to adhere to maximum strictness, so if
your ROM doesn't load, try fixing the header.

Common issues include:
- Oversized/undersized ROMs
- Headers with non-compliant garbage in unused/reserved areas of bytes 9 
through 15
- Battery bit set without setting PRG RAM size
- Battery bit being set in byte 6 and not byte 10 (or vice-versa)
- NTSC/PAL bit being set in byte 9 and not byte 10 (or vice-versa)
