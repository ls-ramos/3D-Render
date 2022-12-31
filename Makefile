build: 
	gcc $(DEBUG) -Wall -std=c99 src/*.c -lSDL2 -o renderer

run:
	./renderer

debug: DEBUG= -g
debug: build

clean:
	rm renderer