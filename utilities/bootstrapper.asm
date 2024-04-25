// when loading using the program loader this part is required to make sure
// that the program loader stays intact while overwriting itself
// !! BOOTSTRAPPER DO NOT CHANGE !!
ENTRY:
  LDAL 0x1000
  LDAH 0x1000
  SETY 0x0000

LOAD_PROGRAM:
  LDBL 0x1000
  SYBL
  INCY
  DEC
  SETB 0x0000
  JGTA LOAD_PROGRAM
  JMPA PROGRAM_ENTRY
// !! BOOTSTRAPPER DO NOT CHANGE !!

PROGRAM_ENTRY: 0x00 // Program will be written from here
