#include "../include/update.h"
#include "../include/game.h"
#include <stdbool.h>
#include <stdlib.h>

static const int dir_rook[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
static const int dir_bishoop[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
static const int dir_queen[8][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                                    {1, 0}, {-1, 0}, {0, 1},  {0, -1}};

static const int knight_moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                                       {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};

static const int pawn_moves[6][2] = {{0, -1}, {1, -1}, {-1, -1},
                                     {0, -2}, {1, 0},  {-1, 0}};

Move **get_possible_move(Game *game, Piece *piece);
bool is_valid_move(int x, int y, Move **possible_moves);
void move_piece(Game *game, Piece *piece, int x, int y);
Move **get_knight_moves(Game *game, Piece *piece);

void free_possible_moves(Move **moves) {
  if (moves == NULL)
    return;

  for (int i = 0; i < 30; i++) {
    if (moves[i] != NULL) {
      free(moves[i]);
    }
  }
  free(moves);
}

bool try_move_selected_piece(Game *game, int dest_x, int dest_y) {
  if (game->selected_piece == NULL) {
    return false;
  }

  if (!is_valid_move(dest_x, dest_y, game->possible_moves)) {
    return false;
  }

  move_piece(game, game->selected_piece, dest_x, dest_y);

  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  game->selected_piece = NULL;

  game->turn = (game->turn == WHITE) ? BLACK : WHITE;

  return true;
}

bool select_piece(Game *game, int x, int y) {
  Piece *clicked = &game->board[y * 8 + x];

  if (clicked->sprite == NULL) {
    return false;
  }
  if (clicked->color != game->turn) {
    return false;
  }

  free_possible_moves(game->possible_moves);

  game->selected_piece = clicked;
  game->possible_moves = get_possible_move(game, clicked);

  return true;
}

void update_game(Game *game) {
  if (!game->pending_click) {
    return;
  }

  int click_x = game->clicked_square_x;
  int click_y = game->clicked_square_y;

  if (try_move_selected_piece(game, click_x, click_y)) {
    game->pending_click = false;
    return;
  }

  if (select_piece(game, click_x, click_y)) {
    game->pending_click = false;
    return;
  }

  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  game->selected_piece = NULL;
  game->pending_click = false;
}

Move **get_moves_rbq(Game *game, Piece *piece);
Move **get_pawn_moves(Game *game, Piece *piece);

Move **get_knight_moves(Game *game, Piece *piece);
Move **get_king_moves(Piece *piece, Move **possible_moves);

Move **get_possible_move(Game *game, Piece *piece) {

  Move **possible_moves = NULL;

  switch (piece->type) {
  case PAWN:
    possible_moves = get_pawn_moves(game, piece);
    break;
  case KNIGHT:
    possible_moves = get_knight_moves(game, piece);
    break;
  case QUEEN:
  case KING:
  case ROOK:
  case BISHOP:
    possible_moves = get_moves_rbq(game, piece);
    break;
  case NO_TYPE:
    break;
  default:
    break;
  }
  return possible_moves;
}

Move **get_moves_rbq(Game *game, Piece *piece) {
  int j = 0;
  const int (*dirs)[2];
  int dir_count;

  Move **moves = malloc(sizeof(Move *) * 30);
  for (int i = 0; i < 30; i++) {
    moves[i] = NULL;
  }

  switch (piece->type) {
  case ROOK:
    dirs = dir_rook;
    dir_count = 4;
    break;
  case BISHOP:
    dirs = dir_bishoop;
    dir_count = 4;
    break;
  case QUEEN:
    dirs = dir_queen;
    dir_count = 8;
    break;
  case KING:
    dirs = dir_queen;
    dir_count = 8;
    break;
  default:
    return NULL;
  };

  for (int d = 0; d < dir_count; d++) {
    for (int dist = 1; dist < 8; dist++) {
      int nx = piece->x + (dirs[d][0] * dist);
      int ny = piece->y + (dirs[d][1] * dist);

      if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
        break;

      Piece *target = &game->board[ny * 8 + nx];

      Move *new_move = malloc(sizeof(Move));
      new_move->x = nx;
      new_move->y = ny;

      if (target->type == NO_TYPE) {
        new_move->spot = HAS_NONE;
        moves[j++] = new_move;
      } else if (target->color != piece->color) {
        new_move->spot = HAS_ENEMY;
        moves[j++] = new_move;
        break;
      } else {
        new_move->spot = HAS_SELF;
        moves[j++] = new_move;
        break;
      }
      if (piece->type == KING) {
        break;
      }
    };
  }
  return moves;
}

Move **get_knight_moves(Game *game, Piece *piece) {
  int j = 0;

  Move **moves = malloc(sizeof(Move *) * 30);

  for (int i = 0; i < 30; i++) {
    moves[i] = NULL;
  }

  for (int i = 0; i < 8; i++) {
    int nx = piece->x + knight_moves[i][0];
    int ny = piece->y + knight_moves[i][1];

    if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8)
      continue;

    Piece *target = &game->board[ny * 8 + nx];

    if (target->isAlive && target->color == piece->color)
      continue;

    Move *new_move = malloc(sizeof(Move));
    new_move->x = nx;
    new_move->y = ny;

    if (target->type == NO_TYPE || !target->isAlive) {
      new_move->spot = HAS_NONE;
    } else {
      new_move->spot = HAS_ENEMY;
    }

    moves[j++] = new_move;
  }

  return moves;
}

Move **get_pawn_moves(Game *game, Piece *piece) {
  int j = 0;

  Move **moves = malloc(sizeof(Move *) * 30);

  for (int i = 0; i < 30; i++) {
    moves[i] = NULL;
  }

  int direction = piece->color == WHITE ? 1 : -1;

  for (int i = 0; i < 6; i++) {
    int nx = piece->x + pawn_moves[i][0];
    int ny = piece->y + pawn_moves[i][1] * direction;

    if (nx >= 8 || nx < 0 || ny >= 8 || ny < 0) {
      continue;
    }

    Piece target = game->board[ny * 8 + nx];

    bool is_adjacent_move = (ny == piece->y);
    if (is_adjacent_move) {
      if (!target.can_enpassant || target.type != PAWN) {
        continue;
      }
      ny = piece->y - direction;
    }

    if (target.color == piece->color && target.isAlive) {
      continue;
    }

    bool is_diagonal_move = nx != piece->x;
    if (is_diagonal_move) {
      if (!target.isAlive) {
        continue;
      }

    } else if (target.isAlive) {
      continue;
    }
    if (piece->moved && abs(ny - piece->y) == 2) {
      continue;
    }
    Move *new_move = malloc(sizeof(Move));
    new_move->x = nx;
    new_move->y = ny;
    new_move->spot = target.isAlive ? HAS_ENEMY : HAS_NONE;
    moves[j++] = new_move;
  }
  return moves;
}

bool is_valid_move(int x, int y, Move **possible_moves) {

  if (possible_moves == NULL)
    return false;
  for (int i = 0; i < 30; i++) {

    if (possible_moves[i] != NULL && possible_moves[i]->x == x &&
        possible_moves[i]->y == y) {
      if (possible_moves[i]->spot != HAS_SELF) {
        return true;
      }
    }
  }
  return false;
}

void move_piece(Game *game, Piece *piece, int x, int y) {
  int old_index = piece->y * 8 + piece->x;
  int new_index = y * 8 + x;

  PieceColor opponent = (piece->color == WHITE) ? BLACK : WHITE;
  for (int i = 0; i < 64; i++) {
    if (game->board[i].color == opponent && game->board[i].type == PAWN) {
      game->board[i].can_enpassant = false;
    }
  }

  bool is_en_passant =
      (piece->type == PAWN && x != piece->x && !game->board[new_index].isAlive);

  if (is_en_passant) {
    int captured_pawn_index = piece->y * 8 + x;
    game->board[captured_pawn_index].isAlive = false;
    game->board[captured_pawn_index].sprite = NULL;
    game->board[captured_pawn_index].type = NO_TYPE;
    game->board[captured_pawn_index].color = NO_COLOR;
  }

  if (game->board[new_index].isAlive) {
    game->board[new_index].isAlive = false;
  }

  bool is_double_pawn_move =
      (piece->type == PAWN && !piece->moved && abs(y - piece->y) == 2);

  game->board[new_index] = game->board[old_index];
  game->board[new_index].x = x;
  game->board[new_index].y = y;
  game->board[new_index].can_enpassant = is_double_pawn_move;
  game->board[new_index].moved = true;

  game->board[old_index].sprite = NULL;
  game->board[old_index].type = NO_TYPE;
  game->board[old_index].color = NO_COLOR;
  game->board[old_index].isAlive = false;
  game->board[old_index].x = old_index % 8;
  game->board[old_index].y = old_index / 8;
  game->board[old_index].isSelected = false;
}
