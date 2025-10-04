# Build for linux
build:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o renderer

run: build
	./renderer

debug:
	gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -g -o renderer


# Build for mac
build-mac:
	gcc -Wall -std=c99 src/*.c -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm -o renderer
	
run-mac: build-mac
	./renderer

debug-mac:
	gcc -Wall -std=c99 src/*.c -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -lm -g -o renderer


# General
clean:
	rm renderer
