#include "display.h"
#include "vector.h"

// declare an array of vectors/points
// define N_POINTS = ( 9*9*9 )
const int N_POINTS = 9 * 9 * 9;
vec3_t cube_points[N_POINTS];

bool is_running = false;

// ---
void setup(void) {
  // allocate the required bytes in memory for the color buffer
  color_buffer =
      (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

  // create a SDL texture that is used to display the color buffer
  color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           window_width, window_height);
  int point_count = 0;

  // start loading my array of vectors
  // from -1 to 1 (in the 9*9*9 cube)

  for (float x = -1; x <= 1; x += 0.25) {
    for (float y = -1; y <= 1; y += 0.25) {
      for (float z = -1; z <= 1; z += 0.25) {
        vec3_t new_point = {.x = x, .y = y, .z = z};
        cube_points[point_count++] = new_point;
      }
    }
  }
}

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

void update(void) {
  // ...
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  draw_dots();
  draw_pixel(20, 20, 0xFFFFFD01);
  draw_rect(300, 200, 300, 150, 0xFFFF00FF);

  /* draw_grid(); */

  render_color_buffer();
  clear_color_buffer(0xFF000000);
  SDL_RenderPresent(renderer);
}

// Main function
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
