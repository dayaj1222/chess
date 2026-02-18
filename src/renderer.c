#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#define SQUARE_SIZE 100
#include "../include/pieces.h"
#include "../include/renderer.h"

void draw_board(SDL_Renderer *renderer) {
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
  return;
}

void draw_hint(SDL_Renderer *renderer, Move *move) {
  int center_x = move->x * 100 + 50;
  int center_y = move->y * 100 + 50;
  int radius = 20;

  switch (move->spot) {
  case HAS_NONE:
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 100);
    break;
  case HAS_SELF:
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 100);
    break;
  case HAS_ENEMY:
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 100);
  }

  for (int w = 0; w < radius * 2; w++) {
    for (int h = 0; h < radius * 2; h++) {
      int dx = radius - w;
      int dy = radius - h;
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderPoint(renderer, center_x + dx, center_y + dy);
      }
    }
  }
}

void draw_hints(Game *game) {

  if (game->selected_piece != NULL) {

    for (int i = 0; i < 30; i++) {
      if (game->possible_moves[i] != NULL) {
        draw_hint(game->renderer, game->possible_moves[i]);
      }
    }
  } else {
    return;
  }
}

bool draw(Game *game) {

  draw_board(game->renderer);
  draw_pieces(game);
  draw_hints(game);

  return true;
}
