#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// Global Variables
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
// ...
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    // create a SDL window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );
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
    // if everything went fine
    return true;
}

// ---
void setup(void) {
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
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
    SDL_RenderPresent(renderer);
}

// --- Freeing ressources
void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
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
