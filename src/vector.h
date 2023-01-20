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

vec3_t add_vec3(vec3_t v1, vec3_t v2);
vec3_t sub_vec3(vec3_t v1, vec3_t v2);
vec3_t vet_prod_vec3(vec3_t v1, vec3_t v2);
float dot_prod_vec3(vec3_t v1, vec3_t v2);
vec3_t esc_prod_vec3(vec3_t v, float escalar);
float length_vec3(vec3_t v);
void normalize_vec3(vec3_t* v);

vec2_t add_vec2(vec2_t v1, vec2_t v2);
vec2_t sub_vec2(vec2_t v1, vec2_t v2);
float dot_prod_vec2(vec2_t v1, vec2_t v2);
vec2_t esc_prod_vec2(vec2_t v, float escalar);
float length_vec2(vec2_t v);
void normalize_vec2(vec2_t* v);

vec3_t rotate_vector_y(vec3_t vector, float angle);
vec3_t rotate_vector_x(vec3_t vector, float angle);
vec3_t rotate_vector_z(vec3_t vector, float angle);

#endif