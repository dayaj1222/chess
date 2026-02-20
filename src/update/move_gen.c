#include "../../include/update/move_gen.h"
#include "../../include/update/move_exec.h"
#include "../../include/update/rules.h"
#include <stdbool.h>
#include <stdlib.h>

#define MAX_MOVES 30

static const int DIR_ROOK[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
static const int DIR_BISHOP[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
static const int DIR_QUEEN[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                    {1, 0}, {-1, 0}, {0, 1},  {0, -1}};

static const int KNIGHT_MOVES[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                       {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

static const int PAWN_MOVES[6][2] = {{0, -1}, {1, -1}, {-1, -1},
                                     {0, -2}, {1, 0},  {-1, 0}};

static Move **alloc_moves(void) {
  Move **moves = malloc(sizeof(Move *) * MAX_MOVES);
  for (int i = 0; i < MAX_MOVES; i++)
    moves[i] = NULL;
  return moves;
}

void free_possible_moves(Move **moves) {
  if (!moves)
    return;
  for (int i = 0; i < MAX_MOVES; i++)
    if (moves[i])
      free(moves[i]);
  free(moves);
}

bool is_valid_move(int x, int y, Move **possible_moves) {
  if (!possible_moves)
    return false;

  for (int i = 0; i < MAX_MOVES; i++) {
    if (possible_moves[i] && possible_moves[i]->x == x &&
        possible_moves[i]->y == y && possible_moves[i]->spot != HAS_SELF)
      return true;
  }
  return false;
}

Move **get_moves_rbq(Game *game, Piece *piece, bool x_ray_king) {
  const int (*dirs)[2];
  int dir_count;
  switch (piece->type) {
  case ROOK:
    dirs = DIR_ROOK;
    dir_count = 4;
    break;
  case BISHOP:
    dirs = DIR_BISHOP;
    dir_count = 4;
    break;
  case QUEEN:
    dirs = DIR_QUEEN;
    dir_count = 8;
    break;
  case KING:
    dirs = DIR_QUEEN;
    dir_count = 8;
    break;
  default:
    return NULL;
  }
  Move **moves = alloc_moves();
  int j = 0;
  for (int d = 0; d < dir_count; d++) {
    for (int dist = 1; dist < 8; dist++) {
      int nx = piece->x + dirs[d][0] * dist;
      int ny = piece->y + dirs[d][1] * dist;
      if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
        break;

      if (piece->type == KING && x_ray_king == false &&
          is_square_attacked(game, nx, ny, piece->color))
        break;

      Piece *target = &game->board[ny * 8 + nx];
      Move *m = malloc(sizeof(Move));
      m->x = nx;
      m->y = ny;
      if (target->type == NO_TYPE) {
        m->spot = HAS_NONE;
        moves[j++] = m;
      } else if (x_ray_king && target->type == KING) {
        m->spot = HAS_NONE;
        moves[j++] = m;
      } else {
        m->spot = (target->color != piece->color) ? HAS_ENEMY : HAS_SELF;
        moves[j++] = m;
        break;
      }
      if (piece->type == KING)
        break;
    }
  }
  return moves;
}

Move **get_knight_moves(Game *game, Piece *piece) {
  Move **moves = alloc_moves();
  int j = 0;

  for (int i = 0; i < 8; i++) {
    int nx = piece->x + KNIGHT_MOVES[i][0];
    int ny = piece->y + KNIGHT_MOVES[i][1];

    if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
      continue;

    Move *m = malloc(sizeof(Move));
    m->x = nx;
    m->y = ny;

    Piece *target = &game->board[ny * 8 + nx];
    if (target->isAlive && target->color == piece->color) {
      m->spot = HAS_SELF;
      moves[j++] = m;
      continue;
    }

    m->spot =
        (target->type == NO_TYPE || !target->isAlive) ? HAS_NONE : HAS_ENEMY;

    moves[j++] = m;
  }

  return moves;
}

Move **get_pawn_moves(Game *game, Piece *piece) {
  Move **moves = alloc_moves();
  int j = 0;

  int direction = (piece->color == WHITE) ? 1 : -1;

  for (int i = 0; i < 6; i++) {
    int nx = piece->x + PAWN_MOVES[i][0];
    int ny = piece->y + PAWN_MOVES[i][1] * direction;

    if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
      continue;

    Piece target = game->board[ny * 8 + nx];

    bool adjacent = (ny == piece->y);
    if (adjacent) {
      if (!target.can_enpassant || target.type != PAWN)
        continue;
      ny = piece->y - direction;
    }

    if (target.color == piece->color && target.isAlive)
      continue;

    bool diagonal = nx != piece->x;

    if (diagonal && !target.isAlive)
      continue;

    if (!diagonal && target.isAlive)
      continue;

    if (piece->moved && abs(ny - piece->y) == 2)
      continue;

    Move *m = malloc(sizeof(Move));
    m->x = nx;
    m->y = ny;
    m->spot = target.isAlive ? HAS_ENEMY : HAS_NONE;

    moves[j++] = m;
  }

  return moves;
}

Move **get_pawn_attack_moves(Game *game, Piece *piece) {
  Move **moves = alloc_moves();
  int j = 0;
  int direction = (piece->color == WHITE) ? -1 : 1;

  int diagonals[2][2] = {{-1, direction}, {1, direction}};

  for (int i = 0; i < 2; i++) {
    int nx = piece->x + diagonals[i][0];
    int ny = piece->y + diagonals[i][1];

    if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
      continue;

    Piece *target = &game->board[ny * 8 + nx];
    Move *m = malloc(sizeof(Move));
    m->x = nx;
    m->y = ny;
    m->spot = (target->type == NO_TYPE || !target->isAlive) ? HAS_NONE
              : (target->color != piece->color)             ? HAS_ENEMY
                                                            : HAS_SELF;
    moves[j++] = m;
  }

  return moves;
}

bool is_move_legal(Game *game, Piece *piece, int tx, int ty) {
  PieceColor color = piece->color;
  Piece board_copy[64];
  memcpy(board_copy, game->board, sizeof(board_copy));

  move_piece(game, piece, tx, ty);
  bool in_check = is_in_check(game, color);

  memcpy(game->board, board_copy, sizeof(board_copy));

  return !in_check;
}

Move **get_possible_move(Game *game, Piece *piece) {
  Move **moves;
  switch (piece->type) {
  case PAWN:
    moves = get_pawn_moves(game, piece);
    break;
  case KNIGHT:
    moves = get_knight_moves(game, piece);
    break;
  case ROOK:
  case BISHOP:
  case QUEEN:
  case KING:
    moves = get_moves_rbq(game, piece, false);
    break;
  default:
    return NULL;
  }

  // filter illegal moves
  for (int i = 0; i < MAX_MOVES; i++) {
    if (!moves[i])
      break;
    if (!is_move_legal(game, piece, moves[i]->x, moves[i]->y)) {
      free(moves[i]);
      moves[i] = NULL;
    }
  }

  return moves;
}
