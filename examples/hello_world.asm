// when loading using the program loader this part is required to make sure
// that the program loader stays intact while overwriting itself
// !! PROGRAM LOADER DO NOT CHANGE !!
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
// !! PROGRAM LOADER DO NOT CHANGE !!

STRING: "hello-world!" 0x0A 0x00

PROGRAM_ENTRY:
  SETX STRING
  SETB 0x0000
LOOP:
  LXAL
  JEQA STOP
  STAL 0x1000
  INCX
  JMPA LOOP

STOP:
  HALT

