#include "../include/input.h"
#include <math.h>

#define SQUARE_SIZE 100

void handle_click(Game *game, SDL_Event *event) {
  int x = event->button.x;
  int y = event->button.y;

  game->clicked_square_x = (int)floor(x / (double)SQUARE_SIZE);
  game->clicked_square_y = (int)floor(y / (double)SQUARE_SIZE);

  game->pending_click = true;
}
