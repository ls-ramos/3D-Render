#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

typedef struct {
    vec3_t direction;
} light_t;

extern light_t light;

uint32_t change_color_intensity(uint32_t color, float intensity);

#endif