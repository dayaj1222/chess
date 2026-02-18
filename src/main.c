#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_mouse.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/game.h"
#include "../include/input.h"
#include "../include/pieces.h"
#include "../include/renderer.h"
#include "../include/update.h"

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
void handle_click(Game *game, SDL_Event *event);

int main() {
  Game *game = malloc(sizeof(Game));
  game->possible_moves = NULL;
  game->renderer = NULL;
  game->window = NULL;
  game->current_scene = PLAYING;
  game->running = true;
  game->pending_click = false;
  game->clicked_square_x = -1;
  game->clicked_square_y = -1;
  game->selected_piece = NULL;
  game->turn = WHITE;

  init_game(game);
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
        handle_click(game, &event);
      }
    }
    SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    update_game(game);
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
  free(game->possible_moves);
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
  SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
  return SDL_APP_CONTINUE;
}
