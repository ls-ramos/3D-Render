build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o renderer

run:
	./renderer

debug:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -g -o renderer

clean:
	rm renderer
