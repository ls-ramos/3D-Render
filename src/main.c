#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "upng.h"
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include "clipping.h"

#ifndef  M_PI
#define  M_PI  3.1415926535897932384626433
#endif

#define MAX_TRIANGLES_PER_MESH 10000
///////////////////////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
///////////////////////////////////////////////////////////////////////////////
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

///////////////////////////////////////////////////////////////////////////////
// Global variables for execution status and game loop
///////////////////////////////////////////////////////////////////////////////
bool is_running = false;
int previous_frame_time = 0;
float delta_time = 0;

mat4_t proj_matrix;
mat4_t view_matrix;

///////////////////////////////////////////////////////////////////////////////
// Setup function to initialize variables and game objects
///////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // Initialize render mode and triangle culling method
    render_method = RENDER_TRIANGLE_TEXTURED;
    cull_method = CULL_BACKFACE;

    // Allocate the required memory in bytes to hold the color buffer
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    // Initialize the perspective projection matrix
    float fov = M_PI / 3.0; // the same as 180/3, or 60deg
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    init_frustum_planes(fov, znear, zfar);

    // Loads the vertex and face values for the mesh data structure
    // load_cube_mesh_data();
    load_obj_file_data("./assets/cube.obj");

    // mesh_texture = (uint32_t *) REDBRICK_TEXTURE;
    load_png_texture_data("./assets/cube.png");
}


bool is_moving_forward = false;
bool is_moving_back = false;
bool is_moving_left = false;
bool is_moving_right = false;
bool is_moving_up = false;
bool is_moving_down = false;

int velocity = 1;

vec3_t camera_right = { .x = 1, .y = 0, .z = 0 };
vec3_t camera_up = { .x = 0, .y = 1, .z = 0 };
vec3_t camera_forward = { .x = 0, .y = 0, .z = 1 };

int mouse_x = 0;
int mouse_y = 0;

///////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    is_running = false;
                if (event.key.keysym.sym == SDLK_1)
                    render_method = RENDER_WIRE_VERTEX;
                if (event.key.keysym.sym == SDLK_2)
                    render_method = RENDER_WIRE;
                if (event.key.keysym.sym == SDLK_3)
                    render_method = RENDER_FILL_TRIANGLE;
                if (event.key.keysym.sym == SDLK_4)
                    render_method = RENDER_FILL_TRIANGLE_WIRE;
                if (event.key.keysym.sym == SDLK_5)
                    render_method = RENDER_TRIANGLE_TEXTURED;
                if (event.key.keysym.sym == SDLK_6)
                    render_method = RENDER_TRIANGLE_TEXTURED_WIRE;
                if (cull_method == CULL_NONE && event.key.keysym.sym == SDLK_c){
                    cull_method = CULL_BACKFACE;
                    break;
                }
                if (cull_method == CULL_BACKFACE && event.key.keysym.sym == SDLK_c)
                    cull_method = CULL_NONE;            
                break;
        }

        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_w)
                    is_moving_forward = event.type == SDL_KEYDOWN;
                if (event.key.keysym.sym == SDLK_s)
                    is_moving_back = event.type == SDL_KEYDOWN;
                if (event.key.keysym.sym == SDLK_a)
                    is_moving_left = event.type == SDL_KEYDOWN;
                if (event.key.keysym.sym == SDLK_d)
                    is_moving_right = event.type == SDL_KEYDOWN;
                if (event.key.keysym.sym == SDLK_e)
                    is_moving_up = event.type == SDL_KEYDOWN;
                if (event.key.keysym.sym == SDLK_q)
                    is_moving_down = event.type == SDL_KEYDOWN;  
                break;
        }
        
    }

    if(is_moving_forward) {
        vec3_t forward = vec3_mul(camera_forward, velocity * delta_time);
        camera.position = vec3_add(camera.position, forward);
    }
    if(is_moving_back) {
        vec3_t backward = vec3_mul(camera_forward, -velocity * delta_time);
        camera.position = vec3_add(camera.position, backward);
    }
    if(is_moving_left) {
        vec3_t left = vec3_mul(camera_right, -velocity * delta_time);
        camera.position = vec3_add(camera.position, left);
    }
    if(is_moving_right) {
        vec3_t right = vec3_mul(camera_right, velocity * delta_time);
        camera.position = vec3_add(camera.position, right);
    }
    if(is_moving_up) {
        vec3_t up = vec3_mul(camera_up, -velocity * delta_time);
        camera.position = vec3_add(camera.position, up);
    }
    if(is_moving_down) {
        vec3_t down = vec3_mul(camera_up, velocity * delta_time);
        camera.position = vec3_add(camera.position, down);
    }

    SDL_GetMouseState(&mouse_x, &mouse_y);

    SDL_WarpMouseInWindow(window, window_width / 2, window_height / 2);
    float mouse_sensitivity = 0.001;
    if (mouse_x != 0) {
        camera.yaw += mouse_sensitivity * ((float) window_width / 2 - mouse_x);
    }
    if (mouse_y != 0) {
        camera.pitch += mouse_sensitivity * ((float) window_height / 2 - mouse_y);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;

    previous_frame_time = SDL_GetTicks();

    // Change the mesh scale, rotation, and translation values per animation frame
    // mesh.rotation.x += 0.5 * delta_time;
    // mesh.rotation.y += 0.5 * delta_time;
    // mesh.rotation.z += 0.5 * delta_time;
    mesh.translation.z = 5.0;

    // camera.position.y += 0.5 * delta_time;
    // camera.position.x += 0.5 * delta_time;

    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    
    mat4_t rotation_target_x = mat4_make_rotation_x(-camera.pitch);
    mat4_t rotation_target_y = mat4_make_rotation_y(-camera.yaw);
    mat4_t rotation_target = mat4_mul_mat4(rotation_target_y,rotation_target_x);
    vec3_t camera_target = vec3_from_vec4(mat4_mul_vec4(rotation_target, vec4_from_vec3(camera.direction)));
    
    camera_right = vec3_cross(camera_up, camera_target);
    vec3_normalize(&camera_right);

    camera_forward = vec3_cross(camera_right, camera_up);
    vec3_normalize(&camera_forward);

    camera_target = vec3_add(camera.position, camera_target);

    view_matrix = mat4_point_at(camera.position, camera_target, camera_up);
    
    // Loop all triangle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for (int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];

        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a World Matrix combining scale, rotation, and translation matrices
            mat4_t world_matrix = mat4_identity();

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            // Multiply the world matrix by the original vector
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }


        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

        // Get the vector subtraction of B-A and C-A
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        vec3_normalize(&vector_ab);
        vec3_normalize(&vector_ac);

        // Compute the face normal (using cross product to find perpendicular)
        vec3_t normal = vec3_cross(vector_ab, vector_ac);
        vec3_normalize(&normal);
        
        vec3_t normal_light = light.direction;
        vec3_normalize(&normal_light);

        // Change color based on the normal direction
        float dot_normal_light = -vec3_dot(normal, normal_light);

        // TODO: I should also chnage the color of the texture based on the dot product
        mesh_face.color = change_color_intensity(mesh_face.color, dot_normal_light);
        
        // Backface culling test to see if the current face should be projected
        if (cull_method == CULL_BACKFACE) {
            // Find the vector between vertex A in the triangle and the camera origin
            vec3_t origin = { .x = 0, .y = 0, .z = 0}; // Camera is always at the origin in this case (Point at camera)
            vec3_t camera_ray = vec3_sub(origin, vector_a);

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = vec3_dot(normal, camera_ray);
            
            // Bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        polygon_t polygon = create_polygon_from_triangle(
            vector_a,
            vector_b,
            vector_c
        );


        clip_polygon(&polygon);

        int num_triangles = polygon.num_vertices - 2;
        triangle_t triangles[num_triangles];
        create_triangles_from_polygon(polygon, triangles);

        for(int n_triangle = 0; n_triangle < num_triangles; n_triangle++){

            vec4_t projected_points[3];

            // Loop all three vertices to perform projection
            for (int j = 0; j < 3; j++) {
                // Project the current vertex
                projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangles[n_triangle].points[j]);

                // Scale into the view
                projected_points[j].x *= (window_width / 2.0);
                projected_points[j].y *= (window_height / 2.0);

                // Flip y value to account for inverted y in screen coordinates (in screen y grows downwards)
                projected_points[j].y *= -1;

                // Translate the projected points to the middle of the screen
                projected_points[j].x += (window_width / 2.0);
                projected_points[j].y += (window_height / 2.0);
            }

            triangle_t projected_triangle = {
                .points = {
                    { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                    { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                    { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
                },
                .texcoords = {
                    { mesh_face.a_uv.u, mesh_face.a_uv.v },
                    { mesh_face.b_uv.u, mesh_face.b_uv.v },
                    { mesh_face.c_uv.u, mesh_face.c_uv.v },
                },
                .color = mesh_face.color,
            };

            if((int)projected_points[0].y == (int)projected_points[1].y && (int)projected_points[0].y == (int)projected_points[2].y) {
                continue;
            }
            
            if(num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
                triangles_to_render[num_triangles_to_render++] = projected_triangle;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_RenderClear(renderer);

    draw_grid();

    // Loop all projected triangles and render them
    for (int i = 0; i < num_triangles_to_render; i++) {
        triangle_t triangle = triangles_to_render[i];

        // Draw filled triangle
        if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,// vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].y, triangle.points[2].w,// vertex C
                triangle.color
            );
        }

        if (render_method == RENDER_TRIANGLE_TEXTURED || render_method == RENDER_TRIANGLE_TEXTURED_WIRE) {
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, // vertex A
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,// vertex B
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,// vertex C
                mesh_texture
            );
        }

        // Draw triangle wireframe
        if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TRIANGLE_TEXTURED_WIRE) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y, // vertex A
                triangle.points[1].x, triangle.points[1].y, // vertex B
                triangle.points[2].x, triangle.points[2].y, // vertex C
                0xFFFFFFFF
            );
        }

        // Draw triangle vertex points
        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C
        }
    }

    // Reset the number of triangles to render every frame
    num_triangles_to_render = 0;

    render_color_buffer();

    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
///////////////////////////////////////////////////////////////////////////////
void free_resources(void) {
    free(color_buffer);
    free(z_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
    upng_free(png_texture);
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}
