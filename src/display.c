#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

uint32_t *color_buffer = NULL;

bool init_sdl(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL :( \n");
    return false;
  };

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);

  window_width = display_mode.w;
  window_height = display_mode.h;

  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width, window_height, SDL_WINDOW_BORDERLESS);
  if (!window) {
    fprintf(stderr, "Error creating SDL window :( \n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer :(\n");
    return false;
  }

  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)window_width * sizeof(uint32_t));

  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      color_buffer[(window_width * y) + x] = color;
    }
  }
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
    color_buffer[(window_width * y) + x] = color;
  }
}

void draw_grid(uint32_t lineColor, int gridSize) {
  int xJump = 1;
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x += xJump) {
      if (y % gridSize == 0) {
        xJump = 1;
      } else {
        xJump = gridSize;
      }
      draw_pixel(x, y, lineColor);
    }
  }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
  int delta_x = x1 - x0;
  int delta_y = y1 - y0;

  int longer_side =
      (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

  float inc_x = delta_x / (float)longer_side;
  float inc_y = delta_y / (float)longer_side;

  float current_x = x0;
  float current_y = y0;

  for (int i = 0; i <= longer_side; i++) {
    draw_pixel(round(current_x), round(current_y), color);
    current_x += inc_x;
    current_y += inc_y;
  }
}

/**
 *        (x0,y0)
 *        /     \
 *       /       \
 *      /         \
 *   (x1,y1)-----(x2,y2)
 */
void draw_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                       uint32_t color) {
  float inc0 = (x1 - x0) / (float)(y1 - y0);
  float inc1 = (x2 - x0) / (float)(y2 - y0);

  float init = x0;
  float end = x0;

  for (int i = y0; i <= y1; i++) {
    draw_line(round(init), i, round(end), i, color);
    init += inc0;
    end += inc1;
  }
}

/**
 *  (x0,y0)----(x1,y1)
 *      \         /
 *       \       /
 *        (x2,y2)
 */
void draw_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color) {
  float inc0 = (x2 - x0) / (float)(y2 - y0);
  float inc1 = (x2 - x1) / (float)(y2 - y1);

  float init = x2;
  float end = x2;

  for (int i = y2; i >= y0; i--) {
    draw_line(round(init), i, round(end), i, color);
    init -= inc0;
    end -= inc1;
  }
}

void swap(int *x, int *y) {
  int tmp = *x;
  *x = *y;
  *y = tmp;
}

/**
 *        (x0,y0)
 *        /     \
 *       /       \
 *      /         \
 *   (x1,y1)    (mx,my)
 *                  \
 *                   \
 *                 (x2,y2)
 */
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint32_t color) {
  if (y1 < y0) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  if (y2 < y1) {
    swap(&x2, &x1);
    swap(&y2, &y1);
  }

  if (y1 < y0) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }

  if (y1 == y2) {
    draw_top_triangle(x0, y0, x1, y1, x2, y2, color);
  } else if (y1 == y0) {
    draw_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
  } else {
    int my = y1;
    int mx = (((y1 - y0) * (x2 - x0)) / (y2 - y0)) + x0;

    draw_top_triangle(x0, y0, x1, y1, mx, my, color);
    draw_bottom_triangle(x1, y1, mx, my, x2, y2, color);
  }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                   uint32_t color) {
  draw_line(x0, y0, x1, y1, color);
  draw_line(x1, y1, x2, y2, color);
  draw_line(x2, y2, x0, y0, color);
}

void draw_rectangle(int xPos, int yPos, int width, int height, uint32_t color) {
  for (int y = yPos; y < yPos + height && y < window_height; y++) {
    for (int x = xPos; x < xPos + width && x < window_width; x++) {
      draw_pixel(x, y, color);
    }
  }
}

void clean_sdl() {
  free(color_buffer);
  SDL_DestroyTexture(color_buffer_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}