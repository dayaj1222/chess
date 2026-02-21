#ifndef MOVE_EXEC_H
#define MOVE_EXEC_H

#include "../game.h"

void move_piece(Game *game, Piece *piece, int x, int y);
void apply_move(Game *game, Piece *piece, int x, int y);

#endif
