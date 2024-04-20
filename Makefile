


all: example_programs

build:
	mkdir build

build/examples: build
	mkdir build/examples

example_programs: build/examples
	gcc ./examples/incrementer.c -o ./build/examples/incrementer
	gcc ./examples/hello_world.c -o ./build/examples/hello_world

clean:
	rm -rf ./build
