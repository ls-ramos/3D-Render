#include <math.h>
#include "clipping.h"

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | | 
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | | 
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void init_frustum_planes(float fov, float z_near, float z_far) {
	float cos_half_fov = cos(fov / 2);
	float sin_half_fov = sin(fov / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2) {
    polygon_t polygon = {
        .vertices = { v0, v1, v2 },
        .num_vertices = 3
    };
    return polygon;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    if (polygon->num_vertices <= 0) return;

    plane_t clipping_plane = frustum_planes[plane];

    vec3_t new_vertices[MAX_NUM_POLY_VERTICES];
    int new_num_vertices = 0;

    vec3_t* vertices = polygon->vertices;
    int num_vertices = polygon->num_vertices;

    vec3_t prev_vertex = vertices[num_vertices - 1];
    float prev_distance = vec3_dot(
        vec3_sub(prev_vertex, clipping_plane.point),
        clipping_plane.normal
    );
    int prev_inside = (prev_distance >= 0);

    for (int i = 0; i < num_vertices; i++) {
        vec3_t curr_vertex = vertices[i];
        float curr_distance = vec3_dot(
            vec3_sub(curr_vertex, clipping_plane.point),
            clipping_plane.normal
        );
        int curr_inside = (curr_distance >= 0);

        if (prev_inside && curr_inside) {
            if (new_num_vertices < MAX_NUM_POLY_VERTICES) {
                new_vertices[new_num_vertices++] = curr_vertex;
            }
        } else if (prev_inside && !curr_inside) {
            // Edge goes from inside to outside: add intersection only
            float t = prev_distance / (prev_distance - curr_distance);
            vec3_t direction = vec3_sub(curr_vertex, prev_vertex);
            vec3_t intersection = vec3_add(prev_vertex, vec3_mul(direction, t));
            if (new_num_vertices < MAX_NUM_POLY_VERTICES) {
                new_vertices[new_num_vertices++] = intersection;
            }
        } else if (!prev_inside && curr_inside) {
            // Edge goes from outside to inside: add intersection and current
            float t = prev_distance / (prev_distance - curr_distance);
            vec3_t direction = vec3_sub(curr_vertex, prev_vertex);
            vec3_t intersection = vec3_add(prev_vertex, vec3_mul(direction, t));
            if (new_num_vertices < MAX_NUM_POLY_VERTICES) {
                new_vertices[new_num_vertices++] = intersection;
            }
            if (new_num_vertices < MAX_NUM_POLY_VERTICES) {
                new_vertices[new_num_vertices++] = curr_vertex;
            }
        }

        prev_vertex = curr_vertex;
        prev_distance = curr_distance;
        prev_inside = curr_inside;
    }

    // Write back the results
    polygon->num_vertices = new_num_vertices;
    for (int i = 0; i < new_num_vertices; i++) {
        polygon->vertices[i] = new_vertices[i];
    }
}

void clip_polygon(polygon_t* polygon) {
    clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}
