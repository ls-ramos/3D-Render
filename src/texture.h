
#ifndef TEXTURE_H
#define TEXTURE_H
#include "upng.h"

typedef struct {
    float u;
    float v;
} txt2_t;

extern upng_t* png_texture;
extern uint32_t* mesh_texture;
extern int texture_width;
extern int texture_height;

extern const uint8_t REDBRICK_TEXTURE[];
void load_png_texture_data(char* filename);

#endif