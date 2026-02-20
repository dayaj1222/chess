#ifndef UPDATE_H
#define UPDATE_H

#include "../game.h"

void update_game(Game *game);

bool select_piece(Game *game, int x, int y);
bool try_move_selected_piece(Game *game, int dest_x, int dest_y);

#endif
