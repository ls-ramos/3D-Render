#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "vector.h"

#define FRAME_RATE 60
#define FRAME_TARGET_TIME 1000 / FRAME_RATE

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

void swap(int *x, int *y);

void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color);

void draw_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                       uint32_t color);
void draw_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color);
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color);

void draw_grid(uint32_t lineColor, int gridSize);
void draw_rectangle(int xPos, int yPos, int width, int height, uint32_t color);
#endif