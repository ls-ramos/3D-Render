#include "display.h"
#include "triangle.h"
#include "vector.h"

void int_swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void float_swap(float* a, float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)------(Mx,My)
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color) {
    if(y0 == y1 && y1 == y2) {
        return;
        printf("Warning: Degenerate triangle\n");
    };
    
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    vec4_t A = {x0, y0, z0, w0};
    vec4_t B = {x1, y1, z1, w1};
    vec4_t C = {x2, y2, z2, w2};

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    // draw flat bottom triangle
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    for(int y = y0; y <= y1; y++){
        float x_start = x0 + (y - y0) * inv_slope_1;
        float x_end = x0 + (y - y0) * inv_slope_2;

        if (x_start > x_end) {
            float_swap(&x_start, &x_end);
        }

        for(int x = x_start; x <= x_end; x++){
            vec2_t P = {x, y};
            draw_filled_pixel(P, A, B, C, color);
        }
    }

    //draw flat top triangle
    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / (y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    for(int y = y1; y <= y2; y++){
        float x_start = x1 + (y - y1) * inv_slope_1;
        float x_end = x0 + (y - y0) * inv_slope_2;

        if (x_start > x_end) {
            float_swap(&x_start, &x_end);
        }

        for(int x = x_start; x <= x_end; x++){
            vec2_t P = {x, y};
            draw_filled_pixel(P, A, B, C, color);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a triangle using three raw line calls
///////////////////////////////////////////////////////////////////////////////
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0, int x1, int y1,  float z1, float w1, float u1, float v1, int x2, int y2,  float z2, float w2, float u2, float v2, uint32_t* texture) {
    
    if(y0 == y1 && y1 == y2) {
        return;
        printf("Warning: Degenerate triangle\n");
    };
    
    if (y0 > y1) {
        int_swap(&y0, &y1);
        float_swap(&v0, &v1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);

        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        float_swap(&v1, &v2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        float_swap(&v0, &v1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    // Flip the y-axis when loading from obj file
    v0 = 1 - v0;
    v1 = 1 - v1;
    v2 = 1 - v2;

    vec4_t A = {x0, y0, z0, w0};
    vec4_t B = {x1, y1, z1, w1};
    vec4_t C = {x2, y2, z2, w2};
    txt2_t uvA = {u0, v0};
    txt2_t uvB = {u1, v1};
    txt2_t uvC = {u2, v2};

    float inv_slope_1 = 0;
    float inv_slope_2 = 0;
    // draw flat bottom triangle
    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    for(int y = y0; y <= y1; y++){
        float x_start = x0 + (y - y0) * inv_slope_1;
        float x_end = x0 + (y - y0) * inv_slope_2;

        if (x_start > x_end) {
            float_swap(&x_start, &x_end);
        }

        for(int x = x_start; x <= x_end; x++){
            vec2_t P = {x, y};
            draw_textured_pixel(P, A, B, C, uvA, uvB, uvC, texture);
        }
    }
    //draw flat top triangle
    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / (y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / (y2 - y0);
    for(int y = y1; y <= y2; y++){
        float x_start = x1 + (y - y1) * inv_slope_1;
        float x_end = x0 + (y - y0) * inv_slope_2;

        if (x_start > x_end) {
            float_swap(&x_start, &x_end);
        }

        for(int x = x_start; x <= x_end; x++){
            vec2_t P = {x, y};
            draw_textured_pixel(P, A, B, C, uvA, uvB, uvC, texture);
        }
    }
}
