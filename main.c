#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 800
#define WIDTH 800
#define SQUARE_SIZE 100

typedef struct Piece {
  int x;
  int y;
  int (*draw)(int, int);
} Piece;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} Game;

// Declaration
SDL_AppResult init_game(Game *game);
bool draw(SDL_Renderer *renderer);
bool handle_click(SDL_Event *events);

int main() {

  Game *game = malloc(sizeof(Game));
  game->renderer = NULL;
  game->window = NULL;

  init_game(game);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }

      bool is_keypress = event.type == SDL_EVENT_KEY_DOWN;
      bool is_required_key =
          event.key.key == SDLK_ESCAPE || event.key.key == SDLK_Q;

      if (is_keypress && is_required_key) {
        running = false;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        handle_click(&event);
      }
    }
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    draw(game->renderer);

    SDL_RenderPresent(game->renderer);
  }

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

bool draw_board(SDL_Renderer *renderer);
bool draw(SDL_Renderer *renderer) {

  draw_board(renderer);
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
