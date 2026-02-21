#include "../../include/update/move_exec.h"
#include "../../include/update/rules.h"
#include <stdio.h>
#include <stdlib.h>

void apply_move(Game *game, Piece *piece, int x, int y) {
  int old_i = piece->y * 8 + piece->x;
  int new_i = y * 8 + x;

  // save before zeroing
  PiecesType piece_type = piece->type;
  int piece_old_x = piece->x;

  // en passant
  bool en_passant =
      (piece_type == PAWN && x != piece_old_x && !game->board[new_i].isAlive);
  if (en_passant) {
    int cap_i = piece->y * 8 + x;
    game->board[cap_i] = (Piece){0};
    game->board[cap_i].x = cap_i % 8;
    game->board[cap_i].y = cap_i / 8;
    game->board[cap_i].type = NO_TYPE;
    game->board[cap_i].color = NO_COLOR;
  }

  bool double_step =
      (piece_type == PAWN && !piece->moved && abs(y - piece->y) == 2);

  game->board[new_i] = game->board[old_i];
  game->board[new_i].x = x;
  game->board[new_i].y = y;
  game->board[new_i].can_enpassant = double_step;
  game->board[new_i].moved = true;

  game->board[old_i] = (Piece){0};
  game->board[old_i].x = old_i % 8;
  game->board[old_i].y = old_i / 8;
  game->board[old_i].type = NO_TYPE;
  game->board[old_i].color = NO_COLOR;

  // castling
  bool castling = (piece_type == KING && abs(x - piece_old_x) == 2);
  if (castling) {
    bool kingside = (x > piece_old_x);
    int rook_old_x = kingside ? 7 : 0;
    int rook_new_x = kingside ? 5 : 3;
    int rook_old_i = y * 8 + rook_old_x;
    int rook_new_i = y * 8 + rook_new_x;
    game->board[rook_new_i] = game->board[rook_old_i];
    game->board[rook_new_i].x = rook_new_x;
    game->board[rook_new_i].moved = true;
    game->board[rook_old_i] = (Piece){0};
    game->board[rook_old_i].x = rook_old_x;
    game->board[rook_old_i].y = y;
    game->board[rook_old_i].type = NO_TYPE;
    game->board[rook_old_i].color = NO_COLOR;
  }
}

void move_piece(Game *game, Piece *piece, int x, int y) {
  PieceColor moving_color = piece->color;
  PieceColor opponent = (moving_color == WHITE) ? BLACK : WHITE;

  for (int i = 0; i < 64; i++)
    if (game->board[i].color == opponent && game->board[i].type == PAWN)
      game->board[i].can_enpassant = false;

  apply_move(game, piece, x, y);

  bool promotion = (game->board[y * 8 + x].type == PAWN && (y == 0 || y == 7));
  if (promotion) {
    game->selected_piece = &game->board[y * 8 + x];
    game->current_scene = PROMOTING;
    return;
  }

  game->turn = opponent;

  bool no_moves = has_no_legal_moves(game, opponent);
  if (is_in_check(game, opponent) && no_moves) {
    game->won = moving_color;
    game->current_scene = GAME_OVER;
  } else if (no_moves) {
    game->stalemate = true;
    game->current_scene = GAME_OVER;
  }
}
