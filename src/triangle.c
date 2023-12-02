#include "display.h"
#include "triangle.h"

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
// Draw a filled a triangle with a flat bottom
///////////////////////////////////////////////////////////////////////////////
//
//        (x0,y0)
//          / \
//         /   \
//        /     \
//       /       \
//      /         \
//  (x1,y1)------(x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x1 - x0) / (y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (y2 - y0);

    // Start x_start and x_end from the top vertex (x0,y0)
    float x_start = x0;
    float x_end = x0;

    // Loop all the scanlines from top to bottom
    for (int y = y0; y <= y2; y++) {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled a triangle with a flat top
///////////////////////////////////////////////////////////////////////////////
//
//  (x0,y0)------(x1,y1)
//      \         /
//       \       /
//        \     /
//         \   /
//          \ /
//        (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // Find the two slopes (two triangle legs)
    float inv_slope_1 = (float)(x2 - x0) / (y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (y2 - y1);

    // Start x_start and x_end from the bottom vertex (x2,y2)
    float x_start = x2;
    float x_end = x2;

    // Loop all the scanlines from bottom to top
    for (int y = y2; y >= y0; y--) {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
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
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2) {
        // Draw flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    } else if (y0 == y1) {
        // Draw flat-top triangle
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    } else {
        // Calculate the new vertex (Mx,My) using triangle similarity
        int My = y1;
        int Mx = (((x2 - x0) * (y1 - y0)) / (y2 - y0)) + x0;

        // Draw flat-bottom triangle
        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

        // Draw flat-top triangle
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
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

void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, uint32_t* texture){
    
    if(y0 == y1 && y1 == y2) {
        return;
        printf("Warning: Degenerate triangle\n");
    };
    
    if (y0 > y1) {
        int_swap(&y0, &y1);
        float_swap(&v0, &v1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        float_swap(&v1, &v2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        float_swap(&v0, &v1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
    }
    vec2_t A = {x0, y0};
    vec2_t B = {x1, y1};
    vec2_t C = {x2, y2};
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
