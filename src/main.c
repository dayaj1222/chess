#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/game.h"
#include "../include/pieces.h"

#define HEIGHT 800
#define WIDTH 800
#define SQUARE_SIZE 100

const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

Uint64 framestart;
int frametime;

// Declaration
SDL_AppResult init_game(Game *game);
bool draw(Game *game);
bool handle_click(SDL_Event *events);

int main() {

  Game *game = malloc(sizeof(Game));
  game->renderer = NULL;
  game->window = NULL;
  game->current_scene = PLAYING;
  game->selected_index = -1;
  game->running = true;

  init_game(game);

  bool running = true;
  SDL_Event event;

  init_board(game);

  while (game->running) {
    framestart = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        game->running = false;
      }

      bool is_keypress = event.type == SDL_EVENT_KEY_DOWN;
      bool is_required_key =
          event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q;

      if (is_keypress && is_required_key) {
        game->running = false;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        handle_click(&event);
      }
    }
    SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    draw(game);

    SDL_RenderPresent(game->renderer);

    frametime = SDL_GetTicks() - framestart;

    if (frametime < FRAME_DELAY) {
      (SDL_Delay(FRAME_DELAY - frametime));
    }
  }

  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);
  free(game);
  return EXIT_SUCCESS;
}

// Implementations

SDL_AppResult init_game(Game *game) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Error Initializing SDL: %s \n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  if (!SDL_CreateWindowAndRenderer("Chess", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE,
                                   &game->window, &game->renderer)) {
    printf("Couldn't Create Window/Renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  return SDL_APP_CONTINUE;
}

// Draw helpers
bool draw_board(SDL_Renderer *renderer);

// Main draw Function
bool draw(Game *game) {

  draw_board(game->renderer);
  draw_pieces(game);

  return true;
}

bool draw_board(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  SDL_FRect square = {0, 0, SQUARE_SIZE, SQUARE_SIZE};

  for (int i = 0; i < 8; i++) {
    if (i % 2 == 0) {
      for (int j = 0; j < 8; j++) {
        SDL_RenderFillRect(renderer, &square);
        square.x += 200;
      }
    } else {
      square.x = 100;
      for (int j = 0; j < 8; j++) {
        SDL_RenderFillRect(renderer, &square);
        square.x += 200;
      }
    }
    square.x = 0;
    square.y += 100;
  }
  return true;
}

bool handle_click(SDL_Event *event) {
  int x = event->button.x;
  int y = event->button.y;
  int button = event->button.button;

  if (button == SDL_BUTTON_LEFT) {
    printf("x: %d , y: %d", x, y);
  }
  return true;
}
