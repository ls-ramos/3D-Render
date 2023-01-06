#include "display.h"
#include "mesh.h"
#include "triangle.h"

float fac_scale = 640;
bool is_running = false;

int previous_time_frame = 0;

bool setup(void);
void process_input(void);
void update(void);
void render(void);

vec3_t camera = {.x = 0, .y = 0, .z = -5};

vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};
triangle_t triangles_to_render[N_MESH_FACES];

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

  cube_rotation.x += 0.02;
  cube_rotation.y += 0.02;
  cube_rotation.z += 0.02;

  for (int i = 0; i < N_MESH_FACES; i++) {
    face_t mesh_face = mesh_faces[i];
    vec3_t vertices[3];
    vertices[0] = mesh_vertices[mesh_face.a - 1];
    vertices[1] = mesh_vertices[mesh_face.b - 1];
    vertices[2] = mesh_vertices[mesh_face.c - 1];

    triangle_t transformed_triangle;

    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertice = vertices[j];

      transformed_vertice =
          rotate_vector_x(transformed_vertice, cube_rotation.x);
      transformed_vertice =
          rotate_vector_y(transformed_vertice, cube_rotation.y);
      transformed_vertice =
          rotate_vector_z(transformed_vertice, cube_rotation.z);

      transformed_vertice.z -= camera.z;
      vec2_t projected_vertice = project(transformed_vertice);

      projected_vertice.x =
          (projected_vertice.x * fac_scale) + (window_width / 2);
      projected_vertice.y =
          (projected_vertice.y * fac_scale) + (window_height / 2);

      transformed_triangle.points[j] = projected_vertice;
    }

    triangles_to_render[i] = transformed_triangle;
  }
}

void render(void) {
  for (int i = 0; i < N_MESH_FACES; i++) {
    triangle_t triangle_to_render = triangles_to_render[i];
    uint32_t triangle_color = 0xFF00FF00;

    draw_rectangle(triangle_to_render.points[0].x,
                   triangle_to_render.points[0].y, 4, 4, triangle_color);
    draw_rectangle(triangle_to_render.points[1].x,
                   triangle_to_render.points[1].y, 4, 4, triangle_color);
    draw_rectangle(triangle_to_render.points[2].x,
                   triangle_to_render.points[2].y, 4, 4, triangle_color);
    draw_triangle(
        triangle_to_render.points[0].x, triangle_to_render.points[0].y,
        triangle_to_render.points[1].x, triangle_to_render.points[1].y,
        triangle_to_render.points[2].x, triangle_to_render.points[2].y,
        triangle_color);
  }

  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}
