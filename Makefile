


all:
	gcc ./examples/incrementer.c -o ./build/examples/incrementer
	gcc ./examples/hello_world.c -o ./build/examples/hello_world

clean:
	rm ./build/examples/incrementer
	rm ./build/examples/hello_world
