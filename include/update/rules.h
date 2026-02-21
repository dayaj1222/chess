#ifndef RULES_H
#define RULES_H

#include "../game.h"

bool is_square_attacked(Game *game, int x, int y, PieceColor color);
bool is_in_check(Game *game, PieceColor color);

bool can_castle(Game *game, Piece *king);

bool is_move_legal(Game *game, Piece *piece, int tx, int ty);

bool has_no_legal_moves(Game *game, PieceColor color);
bool is_checkmate(Game *game, PieceColor color);
bool is_stalemate(Game *game, PieceColor color);

#endif
