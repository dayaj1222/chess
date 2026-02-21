#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "../game.h"

Move **get_possible_move(Game *game, Piece *piece);

Move **get_pawn_moves(Game *game, Piece *piece);

Move **get_pawn_attack_moves(Game *game, Piece *piece);
Move **get_knight_moves(Game *game, Piece *piece);
Move **get_moves_rbq(Game *game, Piece *piece);
Move **get_king_moves(Game *game, Piece *piece, bool x_ray_king);

void free_possible_moves(Move **moves);
bool is_valid_move(int x, int y, Move **possible_moves);

#endif
