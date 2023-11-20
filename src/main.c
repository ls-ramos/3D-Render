#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"

bool is_running = false;

int previous_time_frame = 0;

int render_type = 2;
bool cull_on = true;
int projection_type = 0;

mat4_t projection_matrix;

bool setup(void);
void process_input(void);
void update(void);
void render(void);
void cleanup_resources(void);

vec3_t camera = {.x = 0, .y = 0, .z = 0};

vec3_t light = {.x = 1, .y = 1, .z = 1};

triangle_t* triangles_to_render = NULL;

int main(void) {
  is_running = setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  clean_sdl();
  cleanup_resources();
  return 0;
}

bool setup(void) {
  if (!init_sdl()) {
    fprintf(stderr, "Error initializing SDL\n");
    return false;
  }

  color_buffer =
      (uint32_t*)malloc((window_width * window_height) * sizeof(uint32_t));

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

  float fov = 3.141592 / 3;
  float aspect = (float)window_height / (float)window_width;
  float z_near = 0.1f;
  float z_far = 100.0f;
  projection_matrix = mat4_create_perpective(fov, aspect, z_near, z_far);

  // load_obj_file("assets/cube.obj");
  load_cube_mesh();

  return true;
}

// vec2_t project(vec3_t vector) {
//   if (projection_type == 0) {
//     vec2_t projected_vector = {.x = vector.x / vector.z,
//                                .y = vector.y / vector.z};
//     return projected_vector;
//   } else {
//     vec2_t projected_vector = {.x = vector.x, .y = vector.y};
//     return projected_vector;
//   }
// }

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
      } else if (event.key.keysym.sym == SDLK_1) {
        render_type = 0;
      } else if (event.key.keysym.sym == SDLK_2) {
        render_type = 1;
      } else if (event.key.keysym.sym == SDLK_3) {
        render_type = 2;
      } else if (event.key.keysym.sym == SDLK_4) {
        cull_on = true;
      } else if (event.key.keysym.sym == SDLK_5) {
        cull_on = false;
      } else if (event.key.keysym.sym == SDLK_6) {
        projection_type = 0;
      } else if (event.key.keysym.sym == SDLK_7) {
        projection_type = 1;
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

  triangles_to_render = NULL;

  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.01;
  mesh.rotation.z += 0.02;

  // mesh.translation.x += 0.01;
  // mesh.translation.y += 0.01;
  mesh.translation.z = 5;

  mesh.scale.x = 1;
  mesh.scale.y = 1;
  mesh.scale.z = 1;

  mat4_t mesh_x_rotation = mat4_create_rotate_x(mesh.rotation.x);
  mat4_t mesh_y_rotation = mat4_create_rotate_y(mesh.rotation.y);
  mat4_t mesh_z_rotation = mat4_create_rotate_z(mesh.rotation.z);

  mat4_t mesh_translation = mat4_create_translate(
      mesh.translation.x, mesh.translation.y, mesh.translation.z);
  mat4_t mesh_scale =
      mat4_create_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);

  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];
    vec3_t vertices[3];
    vertices[0] = mesh.vertices[mesh_face.a - 1];
    vertices[1] = mesh.vertices[mesh_face.b - 1];
    vertices[2] = mesh.vertices[mesh_face.c - 1];

    vec3_t transformed_vertices[3];

    // Rotating and sending away from the camera
    for (int j = 0; j < 3; j++) {
      vec4_t transformed_vertice = vec4_from_vec3(vertices[j]);

      transformed_vertice = mat4_multiply_vec4(mesh_scale, transformed_vertice);
      transformed_vertice =
          mat4_multiply_vec4(mesh_x_rotation, transformed_vertice);
      transformed_vertice =
          mat4_multiply_vec4(mesh_y_rotation, transformed_vertice);
      transformed_vertice =
          mat4_multiply_vec4(mesh_z_rotation, transformed_vertice);
      transformed_vertice =
          mat4_multiply_vec4(mesh_translation, transformed_vertice);

      transformed_vertices[j] = vec3_from_vec4(transformed_vertice);
    }

    vec3_t vector_ab =
        sub_vec3(transformed_vertices[1], transformed_vertices[0]);
    vec3_t vector_ac =
        sub_vec3(transformed_vertices[2], transformed_vertices[0]);
    normalize_vec3(&vector_ab);
    normalize_vec3(&vector_ac);

    vec3_t normal = vet_prod_vec3(vector_ab, vector_ac);
    normalize_vec3(&normal);

    vec3_t camera_ray = sub_vec3(camera, transformed_vertices[0]);

    float normal_camera = dot_prod_vec3(normal, camera_ray);
    float light_factor = dot_prod_vec3(normal, light);

    // TODO : Adjust color based on light factor

    if (normal_camera < 0 && cull_on) {
      continue;
    }

    vec4_t projected_vertices[3];

    // Projecting, scaling, translating
    for (int j = 0; j < 3; j++) {
      projected_vertices[j] = mat4_vec4_project(
          projection_matrix, vec4_from_vec3(transformed_vertices[j]),
          projection_type);

      projected_vertices[j].x *= window_width / 2.0;
      projected_vertices[j].y *= window_height / 2.0;

      projected_vertices[j].x += window_width / 2.0;
      projected_vertices[j].y += window_height / 2.0;
    }

    triangle_t transformed_triangle = {
        .points = {{projected_vertices[0].x, projected_vertices[0].y},
                   {projected_vertices[1].x, projected_vertices[1].y},
                   {projected_vertices[2].x, projected_vertices[2].y}},
        .color = mesh_face.color,
        .avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z +
                      transformed_vertices[2].z) /
                     3.0};

    array_push(triangles_to_render, transformed_triangle);
  }

  // Sort the triangles to render by their avg_depth
  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    for (int j = i; j < num_triangles; j++) {
      if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
        // Swap the triangles positions in the array
        triangle_t temp = triangles_to_render[i];
        triangles_to_render[i] = triangles_to_render[j];
        triangles_to_render[j] = temp;
      }
    }
  }
}

void cleanup_resources(void) {
  // TODO: clean color buffer here
  array_free(mesh.vertices);
  array_free(mesh.faces);
}

void render(void) {
  int num_triangles = array_length(triangles_to_render);
  for (int i = 0; i < num_triangles; i++) {
    triangle_t triangle_to_render = triangles_to_render[i];
    if (render_type == 1 || render_type == 2) {
      draw_filled_triangle(
          triangle_to_render.points[0].x, triangle_to_render.points[0].y,
          triangle_to_render.points[1].x, triangle_to_render.points[1].y,
          triangle_to_render.points[2].x, triangle_to_render.points[2].y,
          triangle_to_render.color);
    }
    if (render_type == 0 || render_type == 2) {
      draw_triangle(
          triangle_to_render.points[0].x, triangle_to_render.points[0].y,
          triangle_to_render.points[1].x, triangle_to_render.points[1].y,
          triangle_to_render.points[2].x, triangle_to_render.points[2].y,
          0xFFFFFFFF);
    }
  }
  // draw_filled_triangle(400, 500, 100, 500, 600, 700, 0xFFFFFFFF);
  array_free(triangles_to_render);
  render_color_buffer();
  clear_color_buffer(0xFF000000);

  SDL_RenderPresent(renderer);
}
