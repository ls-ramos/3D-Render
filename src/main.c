#include "display.h"

bool is_running = false;

bool setup(void);
void process_input(void);
void update(void);
void render(void);

const int N_POINTS = 9*9*9;
vec3_t cube_points[N_POINTS];

int main(void) {
  is_running = setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  clean_sdl();
  return 0;
}

bool setup(void) {
  if (!init_sdl()) {
    fprintf(stderr, "Error initializing SDL\n");
    return false;
  }

  color_buffer =
      (uint32_t *)malloc((window_width * window_height) * sizeof(uint32_t));

  if (!color_buffer) {
    fprintf(stderr, "Error allocating color buffer\n");
    return false;
  }

  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  if (!color_buffer_texture) {
    fprintf(stderr, "Error creating SDL color buffer texture\n");
    return false;
  }
  
  int count = 0;
  for(float x=-1; x<= -1; x+= 0.25){
   for(float y=-1; y<= -1; y+= 0.25){
      for(float z=-1; z<= -1; z+= 0.25){
        vec3_t new_point = {x, y, z};
        cube_points[count] = new_point;
        count++;
      }
    } 
  }

  return true;
}

void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
    case SDL_QUIT:
      is_running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        is_running = false;
      }
      break;
    default:
      break;
  }
}

void update(void) {
  // TODO: implement
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_grid(0xFFFFFFFF, 25);
  draw_rectangle(600, 20, 100, 200, 0xFF0000FF);

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}
