


all:
	gcc -o app ./examples/incrementer.c -o ./build/examples/incrementer


clean:
	rm ./build/examples/incrementer
