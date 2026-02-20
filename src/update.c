#include "../include/update/update.h"
#include "../include/update/move_exec.h"
#include "../include/update/move_gen.h"

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
