#include "vector.h"

#include <math.h>

vec3_t add_vec3(vec3_t v1, vec3_t v2) {
  vec3_t result = {.x = v1.x + v2.x, .y = v1.y + v2.y, .z = v1.z + v2.z};
  return result;
}
vec3_t sub_vec3(vec3_t v1, vec3_t v2) {
  vec3_t result = {.x = v1.x - v2.x, .y = v1.y - v2.y, .z = v1.z - v2.z};
  return result;
}
vec3_t vet_prod_vec3(vec3_t v1, vec3_t v2) {
  vec3_t result = {.x = (v1.y * v2.z) - (v1.z * v2.y),
                   .y = (v1.z * v2.x) - (v1.x * v2.z),
                   .z = (v1.x * v2.y) - (v1.y * v2.x)};
  return result;
}
float dot_prod_vec3(vec3_t v1, vec3_t v2) {
  return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}
vec3_t esc_prod_vec3(vec3_t v, float escalar) {
  vec3_t result = {.x = v.x * escalar, .y = v.y * escalar, .z = v.z * escalar};
  return result;
}
float length_vec3(vec3_t v) {
  return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

void normalize_vec3(vec3_t* v) {
  float length = length_vec3(*v);
  v->x /= length;
  v->y /= length;
  v->z /= length;
}

vec2_t add_vec2(vec2_t v1, vec2_t v2) {
  vec2_t result = {.x = v1.x + v2.x, .y = v1.y + v2.y};
  return result;
}
vec2_t sub_vec2(vec2_t v1, vec2_t v2) {
  vec2_t result = {.x = v1.x - v2.x, .y = v1.y - v2.y};
  return result;
}
float dot_prod_vec2(vec2_t v1, vec2_t v2) {
  return (v1.x * v2.x) + (v1.y * v2.y);
}
vec2_t esc_prod_vec2(vec2_t v, float escalar) {
  vec2_t result = {.x = v.x * escalar, .y = v.y * escalar};
  return result;
}

float length_vec2(vec2_t v) { return sqrt((v.x * v.x) + (v.y * v.y)); }

void normalize_vec2(vec2_t* v) {
  float length = length_vec2(*v);
  v->x /= length;
  v->y /= length;
}

vec3_t rotate_vector_y(vec3_t vector, float angle) {
  vec3_t rotated_vector = {.x = vector.x * cos(angle) - vector.z * sin(angle),
                           .y = vector.y,
                           .z = vector.x * sin(angle) + vector.z * cos(angle)};

  return rotated_vector;
}
vec3_t rotate_vector_z(vec3_t vector, float angle) {
  vec3_t rotated_vector = {.x = vector.x * cos(angle) - vector.y * sin(angle),
                           .y = vector.x * sin(angle) + vector.y * cos(angle),
                           .z = vector.z};

  return rotated_vector;
}

vec3_t rotate_vector_x(vec3_t vector, float angle) {
  vec3_t rotated_vector = {.x = vector.x,
                           .y = vector.y * cos(angle) - vector.z * sin(angle),
                           .z = vector.y * sin(angle) + vector.z * cos(angle)};

  return rotated_vector;
}
