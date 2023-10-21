#include "mesh.h"

#include <stdio.h>
#include <stdlib.h>

#include "array.h"

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1},  // 1
    {.x = -1, .y = 1, .z = -1},   // 2
    {.x = 1, .y = 1, .z = -1},    // 3
    {.x = 1, .y = -1, .z = -1},   // 4
    {.x = 1, .y = 1, .z = 1},     // 5
    {.x = 1, .y = -1, .z = 1},    // 6
    {.x = -1, .y = 1, .z = 1},    // 7
    {.x = -1, .y = -1, .z = 1}    // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .color = 0xFF00FF00},
    {.a = 1, .b = 3, .c = 4, .color = 0xFF00FF00},
    // right
    {.a = 4, .b = 3, .c = 5, .color = 0xFFFFFF00},
    {.a = 4, .b = 5, .c = 6, .color = 0xFFFFFF00},
    // back
    {.a = 6, .b = 5, .c = 7, .color = 0xFF00FFFF},
    {.a = 6, .b = 7, .c = 8, .color = 0xFF00FFFF},
    // left
    {.a = 8, .b = 7, .c = 2, .color = 0xFF0000FF},
    {.a = 8, .b = 2, .c = 1, .color = 0xFF0000FF},
    // top
    {.a = 2, .b = 7, .c = 5, .color = 0xFFFF0000},
    {.a = 2, .b = 5, .c = 3, .color = 0xFFFF0000},
    // bottom
    {.a = 6, .b = 8, .c = 1, .color = 0xFFFF00FF},
    {.a = 6, .b = 1, .c = 4, .color = 0xFFFF00FF}};

mesh_t mesh = {.vertices = NULL, .faces = NULL, .rotation = {0, 0, 0}};

void load_cube_mesh(void) {
  for (int i = 0; i < N_CUBE_VERTICES; i++) {
    vec3_t vertice = cube_vertices[i];
    array_push(mesh.vertices, vertice);
  }

  for (int i = 0; i < N_CUBE_FACES; i++) {
    face_t face = cube_faces[i];
    array_push(mesh.faces, face);
  }
}

void load_obj_file(char* filename) {
  FILE* fp = fopen(filename, "r");

  if (fp == NULL) {
    fprintf(stderr, "Error: could not open file %s\n", filename);
    return;
  }

  const unsigned MAX_LENGTH = 256;
  char buffer[MAX_LENGTH];

  while (fgets(buffer, MAX_LENGTH, fp)) {
    if (buffer[0] == 'v' && buffer[1] == ' ') {
      int spaceIndex[3];
      int count = 0;

      for (int i = 0; i < MAX_LENGTH; i++) {
        if (buffer[i] == ' ' && count < 3) {
          spaceIndex[count] = i;
          count++;
        }
      }

      vec3_t vertice = {.x = atof(&buffer[spaceIndex[0]]),
                        .y = atof(&buffer[spaceIndex[1]]),
                        .z = atof(&buffer[spaceIndex[2]])};
      array_push(mesh.vertices, vertice);
    } else if (buffer[0] == 'f') {
      int spaceIndex[3];
      int count = 0;

      for (int i = 0; i < MAX_LENGTH; i++) {
        if (buffer[i] == ' ' && count < 3) {
          spaceIndex[count] = i;
          count++;
        }
      }

      face_t face = {.a = atoi(&buffer[spaceIndex[0]]),
                     .b = atoi(&buffer[spaceIndex[1]]),
                     .c = atoi(&buffer[spaceIndex[2]])};

      array_push(mesh.faces, face);
    }
  }

  if (fclose(fp) != 0) {
    fprintf(stderr, "Error closing file: %s\n", filename);
  };
}