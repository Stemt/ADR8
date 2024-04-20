
# ADR8 Emulator

This is an emulator for a fictional 8-bit processor called the ADR8.
The ADR8 ISA is a CISC architecture inspired by the 6502 processor.

## Compilation

Compiling the included examples and programs requries the following software:
- A C compiler
- GNU Make

To build the examples and programs simply run.
```
make
```

## Usage

### Loading a program using the program loader

This repository provides as simple preconfigured emulator application called the `program_loader`.
This application makes use of the `ADR8_SerialBus` device to read in raw program data an load it into the emulated memory, for more information see the [Devices]() section.

This serial bus allows the emulator to interface with streams such as from files and stdin and stdout.
The `program_loader` is configured to read bytes from stdin load these into memory allowing for dynamic loading and execution of programs.

The 'easiest' way as of now to write a program is to write it inside a text file in a hexadecimal representation and then to use the `xxd` utility to convert this hexadecimal to raw bytes.
Taking the `hello_world.hex` as example, it can be loaded like this.
```
xxd -r -p < examples/hello_world.hex | ./build/utilities/program_loader
```
Note that this loaded file must begin with a dword indicating how many bytes are to be loaded into memory.


### Setting up a custom emulator configuration

The emulator comes in the form a header only library `ADR8.h`.

To use this library first define `ADR8_IMPLEMENTATION` in you source file and then include `ADR8.h`.
Important to note is that only one source file in your application can define the implementation.
```
#define ADR8_IMPLEMENTATION
#include "ADR8.h"
```
The emulator consists of three main parts: the CPU core, the memory and the bus.
The memory is where the data and instructions are stored, it is connected to the CPU core via the bus.
The core is the active part of the emulator, it can use the bus to fetch data and instruction from the memory to run programs.

The bus is best zero initialized as follows and doesn't require any further initialization.
```
ADR8_Bus bus = {0};
```

This can the be used to initialize the memory which also requires a size and a mounting address.
The mounting address determines at what bus address the memory starts.
This example define 0x100 (== 256) bytes and is mounted at address 0.
```
ADR8_Memory mem = {0};
ADR8_Memory_init(&mem, &bus, 0x100, 0x0);
```

Then to initialize the CPU simply pass the bus to it.
```
ADR8_Core core = {0};
ADR8_Core_init(&core, &bus);
```

### Writing your program in memory

The system memory is basically just a large array, it can be set by indexing its `data` attribute.
Note that the it may not be indexed outside of its initialized size;
```
mem.data[0x00] = ADR8_Op_JMPR; // this is an instruction
mem.data[0x01] = 0x00;         // this is an operand for the above instruction
```

For more information on the available instructions see the ISA reference or see the `examples` folder for examples;

### Running your program

Each component of the emulator is updated using its `clock` function, this can be done in a loop continuously to make the emulator run like so.
```
while(!core.halt){
  ADR8_Core_clock(&core);
  ADR8_Memory_clock(&mem);
  ADR8_SerialBus_clock(&serial);
}
```
This loop will repeat until the HALT instruction is reached.

## Devices

The ADR8 doesn't just have to be a virtual machine flipping some bits in memory, using devices can allow programs to interact with things outside of the emulator or otherwise extend its capability.

### Serial Bus

The serial bus is a very simple device that can allow a program to read and write from external stream such as stdin and stdout.
It is initialized by passing the streams (`FILE*`), the bus and the mounting address on the bus.
```
ADR8_SerialBus serial = {0};
ADR8_SerialBus_init(&serial,stdin,stdout, &bus, 0x1000);
```

Now when the CPU writes to the bus at 0x1000 it will actually write to stdout and when reading it will read from stdin. 

## ISA Reference

### Terminology

Before reading this reference its important to be familiar with the following terminology.

Term                 | Description
-------------------- | ---------------------------------------
Signed Integer       | A whole number which can be negative
Unsigned Integer     | A whole number which cannot be negative
Word                 | 8-bit binary number
Half-Word            | 4-bit binary number
DWord or Double-Word | 16-bit binary number

### Registers

The ADR8 architecture supports multiple registers with different functionalities.

Register | Description
-------- | ---------------------------------------------------------------------------------
PC       | Program Counter, points at the next instruction to be executed
STK      | Stack Pointer, points at the current location in the stack
A        | Accumulator, results from the ALU are stored here
B        | Argument Register, used as secondary input for ALU operations and for comparisons
X        | Index Register for A, allows for quick memory access when properly used
Y        | Index Register for B, allows for quick memory access when properly used

### Operand Types

Some instructions take different types of operands, the different types will be indicated using letter combinations.

Operand | Type    | Description
------- | ------- | -------------------------------------------------
xxxx    | Any     | Any dword representing some data
mmmm    | Address | An unsigned integer representing a bus address
dd      | Offset  | A signed integer representing an offset in memory

### Instruction Overview

Instruction | Hex Code | Description
----------- | -------- | -------------------------------------------------------------------------
NOP         | 00       | Do nothing
HALT        | 01       | Stop CPU execution
TRAK        | 02       | Transfer content of A to STK
TRAB        | 03       | Transfer content of A to B
TRBA        | 04       | Transfer content of B to A
TRAX        | 05       | Transfer content of A to X
TRXA        | 06       | Transfer content of X to A
TRAY        | 07       | Transfer content of A to Y
TRYA        | 08       | Transfer content of Y to A
SETK        | 09 xxxx  | Set content of STK to xxxx
SETA        | 0A xxxx  | Set content of A to xxxx
SETB        | 0B xxxx  | Set content of B to xxxx
SETX        | 0C xxxx  | Set content of X to xxxx
SETY        | 0D xxxx  | Set content of y to xxxx
JSR         | 0E mmmm  | Push content of PC on the stack and jump to mmmm
RSR         | 0F       | Pop dword of the stack into PC
LDAL        | 10 mmmm  | Load word from mmmm into lower word of A
LDAH        | 11 mmmm  | Load word from mmmm into higher word of A
LDBL        | 12 mmmm  | Load word from mmmm into lower word of B
LDBH        | 13 mmmm  | Load word from mmmm into lower word of B
LXAL        | 14       | Load word from address stored in X into lower word of A
LXAH        | 15       | Load word from address stored in X into higher word of A
LYBL        | 16       | Load word from address stored in Y into lower word of B
LYBH        | 17       | Load word from address stored in Y into higher word of B
LDA         | 1A mmmm  | Load dword from mmmm into A
LDB         | 1B mmmm  | Load dword from mmmm into B
LDX         | 1C mmmm  | Load dword from mmmm into X
LDY         | 1D mmmm  | Load dword from mmmm into Y
LXA         | 1E       | Load dword from address stored in X into A
LYB         | 1F       | Load dword from address stored in Y into B
STAL        | 20 mmmm  | Store lower word of A to mmmm
STAH        | 21 mmmm  | Store higher word of A to mmmm
STBL        | 22 mmmm  | Store lower word of B to mmmm
STBH        | 23 mmmm  | Store higher word of B to mmmm
SXAL        | 24       | Store lower word of A to address stored in X
SXAH        | 25       | Store higher word of A to address stored in X
SYBL        | 26       | Store lower word of B to address stored in Y
SYBH        | 27       | Store higher word of B to address stored in Y
STA         | 2A mmmm  | Store dword from A to mmmm
STB         | 2B mmmm  | Store dword from B to mmmm
STX         | 2C mmmm  | Store dword from X to mmmm
STY         | 2D mmmm  | Store dword from Y to mmmm
SXA         | 2E       | Store dword from A to address stored in X
SYB         | 2F       | Store dword from A to address stored in Y
ADD         | 30       | Add the contents of A and B and store the result in A
SUB         | 31       | Subtract the contents of A and B and store the result in A
MUL         | 32       | Multiply the contents of A and B and store the result in A
DIV         | 33       | Divide the contents of A and B and store the result in A
INC         | 34       | Increment A by one
DEC         | 35       | Decrement A by one
INCX        | 36       | Increment X by one
INCY        | 37       | Increment Y by one
DECX        | 38       | Decrement X by one
DECY        | 39       | Decrement Y by one
JMPR        | 40 dd    | Add dd to PC
JEQR        | 41 dd    | Add dd to PC if the content A is equal to the content of B
JGTR        | 42 dd    | Add dd to PC if the content A is greater than the content of B
JLTR        | 43 dd    | Add dd to PC if the content A is less than the content of B
JMPA        | 44 mmmm  | Set PC to mmmm
JEQA        | 45 mmmm  | Set PC to mmmm if the content A is equal to the content of B
JGTA        | 46 mmmm  | Set PC to mmmm if the content A is greater than the content of B
JLTA        | 47 mmmm  | Set PC to mmmm if the content A is less than the content of B
PUAL        | 50       | Push the lower word of A onto the stack and decrement STK by one
PUAH        | 51       | Push the higher word of A onto the stack and decrement STK by one
PUBL        | 52       | Push the lower word of B onto the stack and decrement STK by one
PUBH        | 53       | Push the higher word of B onto the stack and decrement STK by one
PUA         | 5A       | Push the dword from A onto the stack and decrement STK by two
PUB         | 5B       | Push the dword from B onto the stack and decrement STK by two
PUX         | 5C       | Push the dword from X onto the stack and decrement STK by two
PUY         | 5D       | Push the dword from Y onto the stack and decrement STK by two
POAL        | 60       | Pop word off the stack into the lower word of A and increment STK by one
POAH        | 61       | Pop word off the stack into the higher word of A and increment STK by one
POBL        | 62       | Pop word off the stack into the lower word of B and increment STK by one
POBH        | 63       | Pop word off the stack into the higher word of B and increment STK by one
POA         | 6A       | Pop dword off the stack into A and increment STK by two
POB         | 6B       | Pop dword off the stack into B and increment STK by two
POX         | 6C       | Pop dword off the stack into X and increment STK by two
POY         | 6D       | Pop dword off the stack into Y and increment STK by two
