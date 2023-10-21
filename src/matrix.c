#include "matrix.h"

mat4_t mat4_create_identity() {
  mat4_t m = {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  return m;
}

vec4_t mat4_multiply_vec4(mat4_t m, vec4_t v) {
  vec4_t result = {
      m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
      m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
      m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
      m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w};

  return result;
}

mat4_t mat4_create_scale(float x, float y, float z) {
  mat4_t m = mat4_create_identity();

  m.m[0][0] = x;
  m.m[1][1] = y;
  m.m[2][2] = z;

  return m;
}

mat4_t mat4_create_translate(float x, float y, float z) {
  mat4_t m = mat4_create_identity();

  m.m[0][3] = x;
  m.m[1][3] = y;
  m.m[2][3] = z;

  return m;
}

mat4_t mat4_create_rotate_x(float angle) {
  mat4_t m = mat4_create_identity();

  m.m[1][1] = cos(angle);
  m.m[1][2] = -sin(angle);
  m.m[2][1] = sin(angle);
  m.m[2][2] = cos(angle);

  return m;
}

mat4_t mat4_create_rotate_y(float angle) {
  mat4_t m = mat4_create_identity();

  m.m[0][0] = cos(angle);
  m.m[0][2] = sin(angle);
  m.m[2][0] = -sin(angle);
  m.m[2][2] = cos(angle);

  return m;
}

mat4_t mat4_create_rotate_z(float angle) {
  mat4_t m = mat4_create_identity();

  m.m[0][0] = cos(angle);
  m.m[0][1] = -sin(angle);
  m.m[1][0] = sin(angle);
  m.m[1][1] = cos(angle);

  return m;
}

mat4_t mat4_create_perpective(float fov, float aspect, float znear,
                              float zfar) {
  mat4_t m = {{{0}}};

  m.m[0][0] = aspect * (1.0 / tan(fov / 2.0));
  m.m[1][1] = 1.0 / tan(fov / 2.0);
  m.m[2][2] = zfar / (zfar - znear);
  m.m[2][3] = (-zfar * znear) / (zfar - znear);
  m.m[3][2] = 1.0;

  return m;
}

vec4_t mat4_vec4_project(mat4_t m, vec4_t v, int perspective_type) {
  vec4_t result = mat4_multiply_vec4(m, v);

  if (result.w != 0.0 && perspective_type == 0) {
    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;
  } else if (perspective_type == 1) {
    result.x /= 5.0;
    result.y /= 5.0;
    result.z /= 5.0;
  }

  return result;
}