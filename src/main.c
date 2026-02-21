#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_mouse.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/game.h"
#include "../include/input.h"
#include "../include/renderer.h"
#include "../include/update/move_gen.h"
#include "../include/update/update.h"

#define WIDTH 800
#define HEIGHT 800
#define SQUARE_SIZE 100

static const int FPS = 60;
static const int FRAME_DELAY = 1000 / FPS;

static Uint64 frame_start;
static int frame_time;

static SDL_AppResult init_game(Game *game);
static void handle_events(Game *game);
static void cleanup(Game *game);

int main(void) {
  Game *game = malloc(sizeof(Game));

  game->renderer = NULL;
  game->window = NULL;
  game->possible_moves = NULL;

  game->current_scene = PLAYING;
  game->running = true;
  game->pending_click = false;

  game->clicked_square_x = -1;
  game->clicked_square_y = -1;

  game->selected_piece = NULL;
  game->turn = WHITE;

  game->stalemate = false;
  game->won = NO_COLOR;

  if (init_game(game) != SDL_APP_CONTINUE)
    return EXIT_FAILURE;

  init_board(game);

  while (game->running) {
    frame_start = SDL_GetTicks();

    handle_events(game);

    SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    update_game(game);
    draw(game);

    SDL_RenderPresent(game->renderer);

    frame_time = SDL_GetTicks() - frame_start;

    if (frame_time < FRAME_DELAY)
      SDL_Delay(FRAME_DELAY - frame_time);
  }

  cleanup(game);
  return EXIT_SUCCESS;
}

static void handle_events(Game *game) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT)
      game->running = false;

    if (event.type == SDL_EVENT_KEY_DOWN) {
      if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q)
        game->running = false;
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
      handle_click(game, &event);
  }
}

static void cleanup(Game *game) {
  if (game->possible_moves)
    free_possible_moves(game->possible_moves);

  SDL_DestroyRenderer(game->renderer);
  SDL_DestroyWindow(game->window);

  free(game);
}

static SDL_AppResult init_game(Game *game) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("Error Initializing SDL: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Chess", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE,
                                   &game->window, &game->renderer)) {
    printf("Couldn't Create Window/Renderer: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);

  return SDL_APP_CONTINUE;
}
