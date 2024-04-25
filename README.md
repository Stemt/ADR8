
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

### Writing a program using the assembler

This repository provides a rudimentary assembler program to create ADR8 binaries using a human readable representation of ADR8 instructions plus some extra features.

The assembly language is just defining the instructions as listed int the [ISA Reference](#Instruction-Overview).

For example to set a value int register A the `SETA xxxx` instruction can be used, where `xxxx` can be replaced with the value you wnat to set.

Note that hexadecimal notation is required as the assembler will always interpret a decimal number as a signed word (a value between -128 and 127).
```
0x01    // unsigned word with value of 1
0x0100  // unsigned double word with value 1
1       // signed word with value of 1
0x0A    // unsigned word with value of 10
0x0A00  // unsigned double word with value of 10
10      // signed word with value of 10
```

So to set A properly the following should be used.
```
SETA 0x0100 // set a value of 1 into A
SETA 0x0A00 // set value of 10 into register A (which is A in hexadecimal)
```

Because this assembler is very dumb, if only 1 word is provided to `SETA` where it expected two it will use the next word in the program which is usually not intended behaviour.
```
SETA 10     // Because SETA here is only provided with 1 word it will grab the value of STA, which is not desireable in this case
STA 0x1000
```

This dumbness also allows for funny stuff like this.
```
SETA SETA 0 // this, like earlier examples, sets a value of 10 into memory but only because the instruction SETA happens to be equivalent to 0x0A (i.e. dont do this if you intend your code to be readable)
```

One of the actually useful features of the assembler is labels.
Labels can be used to, as the name implies, label a place in memory.
Imagine for example you have a variable at memory location 0x0022 you wanted to load into register A.
Then you'd normally do something like this:
```
1 // signed word at 0x0022
LDAL 0x0022
```

Now a label would allow you to give that address a name and use it to refer to that address like so:
```
VARIABLE: 1
LDAL VARIABLE
```

But it doesn't just have to be for variables, its also useful for loops.
```
LOOP: // loop that increments a forever!
    INCA
    JMPA LOOP: // note that only absolute jumps can use labels (dumb assembler)
```

Finally to use text in your programs the assembler supports string literals, which is just a sequence of text that will be written to memory.
```
STRING: "this is a string!"
```

For convenience the assemblier puts a word with the value of zero at the end of the string which can be used to check where the string ends.
These are usually referred to as null terminated strings.

### Assembling your program

Once you have completed writing your file, save it with preferably an opriate file extension like `.asm` and pass it to the assembler and specify your output file using the `-o` option like so.
```
./build/utilities/assembler path/to/your/program.asm -o output.bin
```

### Loading a program using the program loader

If you want to actuallly run the program inside the emulator you can use the preconfigured program_loader emulator for this purpose.
This configuration comes with a preloaded program, the bootstrapper, that uses a `SerialBus` to load your program from stdin, for more info on the serial bus see the [Devices](#Devices) section.

There's one problem with this aproach, this bootstrapper is, like the assembler, very dumb.
The bootstrapper in its valiant effort to load your program into memory will overwrite and kill itself before achieving its goal.
To help it along (and prevent it from killing itself) make sure to include the bootstrappers code at the beginning of your program.
That way the bootstrapper still overwrites itself but does so with its own code so it stays intact.

To do this simply add the `-b` option to the assembler like so.
```
./build/utilities/assembler -b path/to/your/program.asm -o output.bin
```

Now you can run your binary by streaming the binary file to the stdin of the program_loader configuration and watch it go!
```
./build/utilities/program_loader < output.bin
```

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
The core is the active part of the emulator, it can use the bus to fetch data and instructions from the memory to run programs.

The bus is best zero initialized as follows and doesn't require any further initialization.
```
ADR8_Bus bus = {0};
```

This can then be used to initialize the memory which also requires a size and a mounting address.
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
