#include "../include/update/update.h"
#include "../include/renderer.h"
#include "../include/update/move_exec.h"
#include "../include/update/move_gen.h"
#include "../include/update/rules.h"

bool try_move_selected_piece(Game *game, int x, int y) {
  if (!game->selected_piece)
    return false;
  if (!is_valid_move(x, y, game->possible_moves))
    return false;
  move_piece(game, game->selected_piece, x, y);
  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  if (game->current_scene != PROMOTING)
    game->selected_piece = NULL;
  return true;
}

static void reset_game(Game *game) {
  free_possible_moves(game->possible_moves);
  game->possible_moves = NULL;
  game->selected_piece = NULL;
  game->turn = WHITE;
  game->won = NO_COLOR;
  game->stalemate = false;
  game->pending_click = false;
  game->is_white_checked = false;
  game->is_black_checked = false;
  game->current_scene = PLAYING;
  init_board(game);
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

static void handle_promotion(Game *game) {
  if (!game->pending_click)
    return;

  int panel_size = 400;
  int panel_x = (800 - panel_size) / 2;
  int panel_y = (800 - panel_size) / 2;
  int sprite_size = 150;
  int padding = (panel_size - sprite_size * 2) / 3;

  PiecesType types[4] = {QUEEN, ROOK, BISHOP, KNIGHT};
  SDL_FRect spots[4] = {
      {panel_x + padding, panel_y + padding, sprite_size, sprite_size},
      {panel_x + padding * 2 + sprite_size, panel_y + padding, sprite_size,
       sprite_size},
      {panel_x + padding, panel_y + padding * 2 + sprite_size, sprite_size,
       sprite_size},
      {panel_x + padding * 2 + sprite_size, panel_y + padding * 2 + sprite_size,
       sprite_size, sprite_size},
  };

  float cx = game->clicked_square_x * 100.0f + 50.0f;
  float cy = game->clicked_square_y * 100.0f + 50.0f;

  for (int i = 0; i < 4; i++) {
    if (cx >= spots[i].x && cx <= spots[i].x + spots[i].w && cy >= spots[i].y &&
        cy <= spots[i].y + spots[i].h) {
      game->selected_piece->type = types[i];
      game->selected_piece->sprite =
          get_texture(game, types[i], game->selected_piece->color);
      game->selected_piece = NULL;
      game->turn = (game->turn == WHITE) ? BLACK : WHITE;
      game->current_scene = PLAYING;
      game->pending_click = false;
      return;
    }
  }

  game->pending_click = false;
}

void update_game(Game *game) {

  if (game->current_scene == PROMOTING) {
    handle_promotion(game);
    return;
  }

  if (game->current_scene == GAME_OVER) {
    if (game->pending_click) {
      reset_game(game);
      game->pending_click = false;
    }
    return;
  }

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
