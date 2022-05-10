#include "array.h"
#include "display.h"
#include "mesh.h"
#include "vector.h"

////////////////////////////////////////////////////////////
// Array of triangles that should be rendered frame by frame
////////////////////////////////////////////////////////////
triangle_t* triangles_to_render = NULL;

// Global variables for execution status and game loop
vec3_t camera_position = {.x = 0, .y = 0, .z = -5};
float fov_factor = 640;
bool is_running = false;
int previous_frame_time = 0;

////////////////////////////////////////////////////////////
// Setup function to initialize variables and game objects
////////////////////////////////////////////////////////////
void setup(void)
{
  // allocate the required bytes in memory for the color buffer
  color_buffer =
      (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);

  // create a SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);

  // load the cube values into the mesh data structure
  /* load_cube_mesh_data(); */
  load_obj_file_data("./assets/f22.obj");

  // ---
  vec3_t a = {2.5, 6.4, 3.0};
  vec3_t b = {-2.2, 1.4, -1.0};

  float a_length = vec3_length(a);
  float b_length = vec3_length(b);

  vec3_t add_ab = vec3_add(a, b);
  // ---
}

////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
////////////////////////////////////////////////////////////
void process_input(void)
{
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
vec2_t project(vec3_t point)
{
  vec2_t projected_point = {.x = (fov_factor * point.x) / point.z,
                            .y = (fov_factor * point.y) / point.z};
  return projected_point;
}

////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
////////////////////////////////////////////////////////////
void update(void)
{
  // wait some time until we reach the target frame time in ms
  int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

  // only delay execution if we are running too fast
  if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
    SDL_Delay(time_to_wait);
  }

  previous_frame_time = SDL_GetTicks();

  // init the array of triangles to render
  triangles_to_render = NULL;

  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.00;
  mesh.rotation.z += 0.00;

  // loop all triangle faces of our cube mesh
  int num_faces = array_length(mesh.faces);
  for (int i = 0; i < num_faces; i++) {
    face_t mesh_face = mesh.faces[i];

    vec3_t face_vertices[3];
    face_vertices[0] = mesh.vertices[mesh_face.a - 1];
    face_vertices[1] = mesh.vertices[mesh_face.b - 1];
    face_vertices[2] = mesh.vertices[mesh_face.c - 1];

    triangle_t projected_triangle;

    vec3_t transformed_vertices[3];

    // loop all three vertices of this current face and apply transformations
    for (int j = 0; j < 3; j++) {
      vec3_t transformed_vertex = face_vertices[j];

      transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
      transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
      transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

      // translate the vertex away from the camera
      transformed_vertex.z -= camera_position.z;

      // Save transformed vertex in the array of transformed vertices
      transformed_vertices[j] = transformed_vertex;
    }

    // Check backface culling
    vec3_t vector_a = transformed_vertices[0]; /*   A   */
    vec3_t vector_b = transformed_vertices[1]; /*  / \  */
    vec3_t vector_c = transformed_vertices[2]; /* C---B */

    // Loop all three vertices to perform the projection
    for (int j = 0; j < 3; j++) {
      // project the current vertex
      vec2_t projected_point = project(transformed_vertices[j]);

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
void render(void)
{
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
// Free the memory that was dynamically allocated
////////////////////////////////////////////////////////////
void free_ressources(void)
{
  free(color_buffer);
  array_free(mesh.faces);
  array_free(mesh.vertices);
}

////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////
int main(void)
{
  is_running = initialize_window();
  setup();

  while (is_running) {
    process_input();
    update();
    render();
  }

  // free memory
  destroy_window();
  free_ressources();

  return 0;
}
