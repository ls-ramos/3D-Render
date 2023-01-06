#include "display.h"
#define N_POINTS 9 * 9 * 9

float fac_scale = 640;
bool is_running = false;
int previous_time_frame = 0;

bool setup(void);
void process_input(void);
void update(void);
void render(void);

vec3_t cube_points[N_POINTS];
vec3_t camera = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};

vec2_t projected_cube_points[N_POINTS];

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
  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {x, y, z};
        cube_points[count] = new_point;
        count++;
      }
    }
  }

  return true;
}

vec2_t project(vec3_t vector) {
  vec2_t projected_vector = {.x = vector.x / vector.z,
                             .y = vector.y / vector.z};

  return projected_vector;
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
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_time_frame);

  if (time_to_wait > 0) {
    SDL_Delay(time_to_wait);
  }

  previous_time_frame = SDL_GetTicks();

  cube_rotation.x += 0.01;
  cube_rotation.y += 0.01;
  cube_rotation.z += 0.01;

  for (int i = 0; i < N_POINTS; i++) {
    vec3_t transformed_vector =
        rotate_vector_x(cube_points[i], cube_rotation.x);
    transformed_vector = rotate_vector_y(transformed_vector, cube_rotation.y);
    transformed_vector = rotate_vector_z(transformed_vector, cube_rotation.z);

    transformed_vector.z -= camera.z;

    projected_cube_points[i] = project(transformed_vector);
  }
}

void render(void) {
  for (int i = 0; i < N_POINTS; i++) {
    vec2_t point = projected_cube_points[i];

    draw_rectangle((point.x * fac_scale) + (window_width / 2),
                   (point.y * fac_scale) + (window_height / 2), 4, 4,
                   0xFFFFFF00);
  }

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}
