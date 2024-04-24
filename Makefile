
CC = gcc
CFLAGS = -Wall -g
ADR8_ASM = ./build/utilities/assembler

ifdef DEBUG
LOG_LEVEL_DEF := -DADR8_LOG_LEVEL=$(DEBUG)
else
LOG_LEVEL_DEF := -DADR8_LOG_LEVEL=0
endif


all: example_programs utility_programs

build/utilities:
	mkdir -p build/utilities

utility_programs: build/utilities
	$(CC) $(CFLAGS) $(LOG_LEVEL_DEF) ./utilities/program_loader.c -o ./build/utilities/program_loader
	$(CC) $(CFLAGS) $(LOG_LEVEL_DEF) ./utilities/assembler.c -o ./build/utilities/assembler

build/examples:
	mkdir -p build/examples

example_programs: build/examples utility_programs
	$(CC) $(CFLAGS) $(LOG_LEVEL_DEF) ./examples/incrementer.c -o ./build/examples/incrementer
	$(CC) $(CFLAGS) $(LOG_LEVEL_DEF) ./examples/hello_world.c -o ./build/examples/hello_world
	$(ADR8_ASM) ./utilities/program_loader.asm ./examples/hello_world.asm -o ./build/examples/hello_world.bin

clean:
	rm -rf ./build
