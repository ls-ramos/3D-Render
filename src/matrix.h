#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>

#include "vector.h"

typedef struct {
  float m[4][4];
} mat4_t;

mat4_t mat4_create_identity();
vec4_t mat4_multiply_vec4(mat4_t m, vec4_t v);

mat4_t mat4_create_perpective(float fov, float aspect, float znear, float zfar);
mat4_t mat4_create_scale(float x, float y, float z);
mat4_t mat4_create_translate(float x, float y, float z);
mat4_t mat4_create_rotate_x(float angle);
mat4_t mat4_create_rotate_y(float angle);
mat4_t mat4_create_rotate_z(float angle);

vec4_t mat4_vec4_project(mat4_t m, vec4_t v, int perspective_type);
#endif