#include "../../include/update/rules.h"
#include "../../include/update/move_gen.h"
#include <stdbool.h>
#include <stdlib.h>

#define MAX_MOVES 30
#define BOARD_SIZE 64

void update_check_state(Game *game) {
  game->is_white_checked = is_in_check(game, WHITE);
  game->is_black_checked = is_in_check(game, BLACK);
}

static Move **get_attacked_squares(Game *game, Piece *piece) {
  Move **attacked_moves = NULL;
  switch (piece->type) {
  case KING:
  case QUEEN:
  case ROOK:
  case BISHOP:
    attacked_moves = get_moves_rbq(game, piece, true);
    break;
  case KNIGHT:
    attacked_moves = get_knight_moves(game, piece);
    break;
  case PAWN:
    attacked_moves = get_pawn_attack_moves(game, piece);
    break;
  case NO_TYPE:
    break;
  }

  return attacked_moves;
}
bool is_square_attacked(Game *game, int x, int y, PieceColor color) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    Piece *p = &game->board[i];

    if (!p->isAlive || p->color == color)
      continue;

    Move **moves = get_attacked_squares(game, p);
    for (int j = 0; j < MAX_MOVES; j++) {
      if (!moves[j])
        break;
      if (moves[j]->x == x && moves[j]->y == y) {
        free_possible_moves(moves);
        return true;
      }
    }
    free_possible_moves(moves);
  }
  return false;
}

bool is_in_check(Game *game, PieceColor color) {
  for (int i = 0; i < 64; i++) {
    if (game->board[i].type == KING && game->board[i].color == color) {
      return is_square_attacked(game, game->board[i].x, game->board[i].y,
                                color);
    }
  }
  return false;
}

bool is_checkmate(Game *game, PieceColor color);
bool is_stalemate(Game *game, PieceColor color);
