#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;

extern uint32_t *color_buffer;

bool init_sdl(void);
void clean_sdl();

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);

void draw_grid(uint32_t lineColor, int gridSize);
void draw_rectangle(int xPos, int yPos, int width, int height, uint32_t color);
#endif