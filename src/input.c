#include "../include/input.h"

#include <math.h>

void handle_click(Game *game, SDL_Event *event) {
  game->clicked_square_x = floor(event->button.x / 100.0);
  game->clicked_square_y = floor(event->button.y / 100.0);
  game->pending_click = true;
}
