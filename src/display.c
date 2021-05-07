#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }
  // use SDL to query what is the fullscreen max width and height
  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  window_width = display_mode.w;
  window_height = display_mode.h;
  // create a SDL window
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       window_width, window_height, SDL_WINDOW_BORDERLESS);
  // is it a valid pointer and do we have content ?
  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  // create a SDL renderer
  renderer = SDL_CreateRenderer(window, -1, 0);
  // is it a valid pointer and do we have content ?
  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }
  // change the video mode to be real fullscreen
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
  // if everything went fine
  return true;
}

// ---

// -- Draw a background grid that fills the entire window.
// -- Lines should be rendered at every row/col multiple of 10.
//
// -- +--+--+--+--+--+--+--+--+--+
// -- |  |  |  |  |  |  |  |  |  |
// -- |__|__|__|__|__|__|__|__|__|
// -- |  |  |  |  |  |  |  |  |  |
// -- |__|__|__|__|__|__|__|__|__|
// -- |  |  |  |  |  |  |  |  |  |
// -- |__|__|__|__|__|__|__|__|__|
// -- |  |  |  |  |  |  |  |  |  |
// -- |  |  |  |  |  |  |  |  |  |
// -- +--+--+--+--+--+--+--+--+--+

void draw_grid(void) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      if (x % 10 == 0 || y % 10 == 0) {
        color_buffer[(window_width * y) + x] = 0xFF444444;
      }
    }
  }
}

// -- Draw small dots instead of lines
void draw_dots(void) {
  for (int y = 0; y < window_height; y += 10) {
    for (int x = 0; x < window_width; x += 10) {
      color_buffer[(window_width * y) + x] = 0xFF444444;
    }
  }
}

// -- Renders a ( filled ) rectangle on the screen
void draw_rect(int x, int y, int width, int height, uint32_t color) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int current_x = x + i;
      int current_y = y + j;
      color_buffer[(window_width * current_y) + current_x] = color;
    }
  }
}

// -- Draw colored pixel
void draw_pixel(int x, int y, uint32_t color) {
  if (x < window_width && y < window_height) {
    color_buffer[(window_width * y) + x] = color;
  }
}

void clear_color_buffer(uint32_t color) {
  for (int y = 0; y < window_height; y++) {
    for (int x = 0; x < window_width; x++) {
      color_buffer[(window_width * y) + x] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer,
                    (int)(window_width * sizeof(uint32_t)));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

// --- Freeing ressources
void destroy_window(void) {
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
