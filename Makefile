build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o renderer

build-mac:
	gcc $(DEBUG) -Wall -std=c99 src/*.c -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm -o renderer
	
run:
	./renderer

debug:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -g -o renderer

clean:
	rm renderer
