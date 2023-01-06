#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
  float x;
  float y;
} vec2_t;

typedef struct {
  float x;
  float y;
  float z;
} vec3_t;

vec3_t rotate_vector_y(vec3_t vector, float angle);
vec3_t rotate_vector_x(vec3_t vector, float angle);
vec3_t rotate_vector_z(vec3_t vector, float angle);

#endif