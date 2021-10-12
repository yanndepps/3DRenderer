#include "array.h"
#include "display.h"
#include "mesh.h"
#include "vector.h"

////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
////////////////////////////////////////////////////////////
triangle_t *triangles_to_render = NULL;

// Global variables for execution status and game loop
vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0};
float fov_factor = 640;
bool is_running = false;
int previous_frame_time = 0;

////////////////////////////////////////////////////////////
// Setup function to initialize variables and game objects
////////////////////////////////////////////////////////////
void setup(void) {
  // allocate the required bytes in memory for the color buffer
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  // create a SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
}

////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
////////////////////////////////////////////////////////////
void process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);

  switch (event.type) {
  case SDL_QUIT:
    is_running = false;
    break;
  case SDL_KEYDOWN:
    if (event.key.keysym.sym == SDLK_ESCAPE)
      is_running = false;
    break;
  }
}

////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a 2D point
////////////////////////////////////////////////////////////
vec2_t project(vec3_t point) {
  vec2_t projected_point = {.x = (fov_factor * point.x) / point.z,
                            .y = (fov_factor * point.y) / point.z};
  return projected_point;
}

////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
////////////////////////////////////////////////////////////
void update(void) {
  // wait some time until we reach the target frame time in ms
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  // only delay execution if we are running too fast
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // init the array of triangles to render
  triangles_to_render = NULL;

  cube_rotation.x += 0.01;
  cube_rotation.y += 0.01;
  cube_rotation.z += 0.01;

  // loop all triangle faces of our cube mesh
  for (int i = 0; i < N_MESH_FACES; i++) {
    face_t mesh_face = mesh_faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh_vertices[mesh_face.a - 1];
    face_vertices[1] = mesh_vertices[mesh_face.b - 1];
    face_vertices[2] = mesh_vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    // loop all three vertices of this current face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);

      // translate the vertex away from the camera
      transformed_vertex.z -= camera_position.z;

      // project the current vertex
      vec2_t projected_point = project(transformed_vertex);

      // scale and translate the projected points to the middle of the screen
      projected_point.x += (window_width / 2);
      projected_point.y += (window_height / 2);

      projected_triangle.points[j] = projected_point;
    }
    // save the projected triangle in the array of triangles to render
    array_push(triangles_to_render, projected_triangle);
  }
}

////////////////////////////////////////////////////////////
// Render function to draw objects on the display
////////////////////////////////////////////////////////////
void render(void) {
  /* draw_grid(); */
  draw_dots();

  // loop all projected triangles and render them
  int num_triangles = array_length(triangles_to_render);

  for (int i = 0; i < num_triangles; i++) {
    triangle_t triangle = triangles_to_render[i];

    // draw vertex points
    draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3,
              0xFFFFFF00); // vertex A
    draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3,
              0xFFFFFF00); // vertex B
    draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3,
              0xFFFFFF00); // vertex C

    // draw unfilled triangles
    draw_triangle(triangle.points[0].x, triangle.points[0].y, // vertex A
                  triangle.points[1].x, triangle.points[1].y, // vertex B
                  triangle.points[2].x, triangle.points[2].y, // vertex C
                  0xFF00FF00);
  }

  // clear the array of triangles to render every frame loop
  array_free(triangles_to_render);

  render_color_buffer();
  clear_color_buffer(0xFF000000);
  SDL_RenderPresent(renderer);
}

////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////
int main(void) {
  is_running = initialize_window();
  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  // free memory
  destroy_window();
  return 0;
}
