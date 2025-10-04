# 3D Render

3d rendering objects with texture in C with SDL.

https://github.com/user-attachments/assets/cce774f1-bcae-482b-b33a-2b18a0ef7d05

## Dependencies

## SDL2

This project depends on SDL2

### Install on MacOS

`brew install sdl2`

### Install on Linux

TODO: Add command here

## Clangd config (VSCode extension for intelisense for c/c++)

### MacOs

```
CompileFlags:
  Add:
    - -I/opt/homebrew/include
    - -I/opt/homebrew/include/SDL2
```

## How to run

1. `make build` or `make build-mac` when on macbook (tested on macbook m1)
2. `make run`
3. Play around with options and movements :)
   
## Options and movement
Keyboard options (KEY - OPTION): 
 ESC - To leave the program
 1 - Render wire with vertexes
 2 - Render only wire
 3 - Render triangles
 4 - Render triangles and wire
 5 - Render texture
 6 - Render texture and wire
 c - Toogle backface culling

Movements (KEY - OPTION):

  w - Move forward
  s - Move backwards
  a - Move left
  d - Move right
  q - Move up
  e - Move down

Move mouse to rotate the camera

## Implemented so far

Without other dependencies than SDL for 3d rendering, as shown in the video, this project at the moment has implemented:

- Rendering objects from obj files.
- Rendering texture of object from png texture.
- Basic camera movement
- Different options of rendering
- Some optimizations (Ex: Backface culling)

## Further improvements

- Remove mouse pointer
- Clipping of polygons outside of view (you will notice that for now if the object is way from the view the program will crash)
- Multiple objects scenes
- Further optimizations
