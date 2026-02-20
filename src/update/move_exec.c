#include "../../include/update/move_exec.h"
#include <stdlib.h>

void move_piece(Game *game, Piece *piece, int x, int y) {
  int old_i = piece->y * 8 + piece->x;
  int new_i = y * 8 + x;

  PieceColor opponent = (piece->color == WHITE) ? BLACK : WHITE;

  for (int i = 0; i < 64; i++)
    if (game->board[i].color == opponent && game->board[i].type == PAWN)
      game->board[i].can_enpassant = false;

  bool en_passant =
      (piece->type == PAWN && x != piece->x && !game->board[new_i].isAlive);

  if (en_passant) {
    int cap_i = piece->y * 8 + x;
    game->board[cap_i].isAlive = false;
    game->board[cap_i].sprite = NULL;
    game->board[cap_i].type = NO_TYPE;
    game->board[cap_i].color = NO_COLOR;
  }

  if (game->board[new_i].isAlive)
    game->board[new_i].isAlive = false;

  bool double_step =
      (piece->type == PAWN && !piece->moved && abs(y - piece->y) == 2);

  game->board[new_i] = game->board[old_i];
  game->board[new_i].x = x;
  game->board[new_i].y = y;
  game->board[new_i].can_enpassant = double_step;
  game->board[new_i].moved = true;

  game->board[old_i].sprite = NULL;
  game->board[old_i].type = NO_TYPE;
  game->board[old_i].color = NO_COLOR;
  game->board[old_i].isAlive = false;
  game->board[old_i].x = old_i % 8;
  game->board[old_i].y = old_i / 8;
  game->board[old_i].isSelected = false;
}
