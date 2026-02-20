#include "../include/update.h"
#include "../include/game.h"
#include <stdbool.h>
#include <stdlib.h>

static const int DIR_ROOK[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
static const int DIR_BISHOP[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
static const int DIR_QUEEN[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                    {1, 0}, {-1, 0}, {0, 1},  {0, -1}};

static const int KNIGHT_MOVES[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                       {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

static const int PAWN_MOVES[6][2] = {{0, -1}, {1, -1}, {-1, -1},
                                     {0, -2}, {1, 0},  {-1, 0}};

static Move **alloc_moves(void) {
  Move **moves = malloc(sizeof(Move *) * 30);
  for (int i = 0; i < 30; i++)
    moves[i] = NULL;
  return moves;
}

void free_possible_moves(Move **moves) {
  if (!moves)
    return;
  for (int i = 0; i < 30; i++)
    if (moves[i])
      free(moves[i]);
  free(moves);
}

bool is_valid_move(int x, int y, Move **possible_moves) {
  if (!possible_moves)
    return false;

  for (int i = 0; i < 30; i++) {
    if (possible_moves[i] && possible_moves[i]->x == x &&
        possible_moves[i]->y == y && possible_moves[i]->spot != HAS_SELF)
      return true;
  }
  return false;
}

Move **get_moves_rbq(Game *game, Piece *piece) {
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

      Piece *target = &game->board[ny * 8 + nx];

      Move *m = malloc(sizeof(Move));
      m->x = nx;
      m->y = ny;

      if (target->type == NO_TYPE) {
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

    Piece *target = &game->board[ny * 8 + nx];
    if (target->isAlive && target->color == piece->color)
      continue;

    Move *m = malloc(sizeof(Move));
    m->x = nx;
    m->y = ny;
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

Move **get_possible_move(Game *game, Piece *piece) {
  switch (piece->type) {
  case PAWN:
    return get_pawn_moves(game, piece);
  case KNIGHT:
    return get_knight_moves(game, piece);
  case ROOK:
  case BISHOP:
  case QUEEN:
  case KING:
    return get_moves_rbq(game, piece);
  default:
    return NULL;
  }
}

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

bool try_move_selected_piece(Game *game, int x, int y) {
  if (!game->selected_piece)
    return false;
  if (!is_valid_move(x, y, game->possible_moves))
    return false;

  move_piece(game, game->selected_piece, x, y);

  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  game->selected_piece = NULL;
  game->turn = (game->turn == WHITE) ? BLACK : WHITE;

  return true;
}

bool select_piece(Game *game, int x, int y) {
  Piece *clicked = &game->board[y * 8 + x];

  if (!clicked->sprite)
    return false;
  if (clicked->color != game->turn)
    return false;

  free_possible_moves(game->possible_moves);

  game->selected_piece = clicked;
  game->possible_moves = get_possible_move(game, clicked);

  return true;
}

void update_game(Game *game) {
  if (!game->pending_click)
    return;

  int x = game->clicked_square_x;
  int y = game->clicked_square_y;

  if (try_move_selected_piece(game, x, y) || select_piece(game, x, y)) {
    game->pending_click = false;
    return;
  }

  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  game->selected_piece = NULL;
  game->pending_click = false;
}
