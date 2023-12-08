#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* color_buffer = NULL;
float* z_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

enum cull_method cull_method;
enum render_method render_method;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Set width and height of the SDL window with the max screen resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create a SDL Window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    return true;
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void clear_z_buffer(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            z_buffer[(window_width * y) + x] = 1.0f;
        }
    }
}

void destroy_window(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_filled_pixel(vec2_t P,vec4_t A,vec4_t B,vec4_t C, uint32_t color) {
    float denominador = (C.x-A.x)*(B.y-A.y) - (C.y-A.y)*(B.x-A.x);
    if (denominador == 0) {
        return;
    }
    float alpha = (((C.x - P.x)*(B.y-P.y)) - ((C.y - P.y)*(B.x-P.x))) / denominador;
    float beta = (((A.x - P.x)*(C.y-P.y)) - ((A.y - P.y)*(C.x-P.x))) / denominador;
    float gamma = 1.0f - alpha - beta;
    if (alpha < 0 || beta < 0 || gamma < 0 || alpha > 1 || beta > 1 || gamma > 1) {
        return;
    }

    float w = (1 / A.w) * alpha + (1 / B.w) * beta + (1 / C.w) * gamma;

    // Adjust w so the closer the pixel is the smaller is the value of w
    w = 1.0f - w;
    int z_buffer_pos = (int) (window_width * P.y) + P.x;

    if (w < z_buffer[z_buffer_pos]) {
        z_buffer[z_buffer_pos] = w;
        draw_pixel(P.x, P.y, color);
    }   
}

void draw_textured_pixel(vec2_t P,vec4_t A,vec4_t B,vec4_t C,txt2_t uvA,txt2_t uvB,txt2_t uvC, uint32_t* texture) {
    float denominador = (C.x-A.x)*(B.y-A.y) - (C.y-A.y)*(B.x-A.x);
    if (denominador == 0) {
        return;
    }
    float alpha = (((C.x - P.x)*(B.y-P.y)) - ((C.y - P.y)*(B.x-P.x))) / denominador;
    float beta = (((A.x - P.x)*(C.y-P.y)) - ((A.y - P.y)*(C.x-P.x))) / denominador;
    float gamma = 1.0f - alpha - beta;
    if (alpha < 0 || beta < 0 || gamma < 0 || alpha > 1 || beta > 1 || gamma > 1) {
        return;
    }

    float u = (uvA.u/A.w) * alpha + (uvB.u/B.w) * beta + (uvC.u/C.w) * gamma;
    float v = (uvA.v/A.w) * alpha + (uvB.v/B.w) * beta + (uvC.v/C.w) * gamma;
    float w = (1 / A.w) * alpha + (1 / B.w) * beta + (1 / C.w) * gamma;

    u /= w;
    v /= w;

    int texture_x = abs((int)(u * (texture_width -1)));
    int texture_y = abs((int)(v * (texture_height -1)));

    // Adjust w so the closer the pixel is the smaller is the value of w
    w = 1.0f - w;
    int z_buffer_pos = (int) (window_width * P.y) + P.x;

    if((texture_width * texture_y) + texture_x > texture_width * texture_height) {
        return;
    } else if ((texture_width * texture_y) + texture_x < 0) {
        return;
    } else if (w < z_buffer[z_buffer_pos]) {
        z_buffer[z_buffer_pos] = w;
        uint32_t texel = texture[(texture_width * texture_y) + texture_x];
        draw_pixel(P.x, P.y, texel);
    }   
}
