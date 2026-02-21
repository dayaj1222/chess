#include <stdbool.h>
#include <stdio.h>

#include "../include/renderer.h"

#define BOARD_SIZE 8
#define BOARD_CELLS 64
#define SQUARE_SIZE 100
#define HINT_RADIUS 20
#define MAX_MOVES 30

static void draw_board(SDL_Renderer *renderer);
static void draw_hint(SDL_Renderer *renderer, const Move *move);
static void draw_hints(Game *game);

SDL_Texture *get_texture(Game *game, PiecesType type, PieceColor color) {
  return game->textures[color][type];
}

void draw_board(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

  SDL_FRect square = {0, 0, SQUARE_SIZE, SQUARE_SIZE};

  for (int row = 0; row < BOARD_SIZE; row++) {
    square.x = (row % 2 == 0) ? 0 : SQUARE_SIZE;

    for (int col = 0; col < BOARD_SIZE; col++) {
      SDL_RenderFillRect(renderer, &square);
      square.x += SQUARE_SIZE * 2;
    }

    square.y += SQUARE_SIZE;
  }
}

static void draw_hint(SDL_Renderer *renderer, const Move *move) {
  int center_x = move->x * SQUARE_SIZE + SQUARE_SIZE / 2;
  int center_y = move->y * SQUARE_SIZE + SQUARE_SIZE / 2;

  switch (move->spot) {
  case HAS_NONE:
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 100);
    break;
  case HAS_SELF:
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 100);
    break;
  case HAS_ENEMY:
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 100);
    break;
  }

  const int radius_sq = HINT_RADIUS * HINT_RADIUS;

  for (int w = 0; w < HINT_RADIUS * 2; w++) {
    for (int h = 0; h < HINT_RADIUS * 2; h++) {
      int dx = HINT_RADIUS - w;
      int dy = HINT_RADIUS - h;

      if ((dx * dx + dy * dy) <= radius_sq)
        SDL_RenderPoint(renderer, center_x + dx, center_y + dy);
    }
  }
}

static void draw_hints(Game *game) {
  if (!game->selected_piece)
    return;

  for (int i = 0; i < MAX_MOVES; i++) {
    if (game->possible_moves[i])
      draw_hint(game->renderer, game->possible_moves[i]);
  }
}

static void draw_promotion_menu(Game *game) {

  if (game->selected_piece == NULL) {
    return;
  }
  int panel_size = 400;
  int panel_x = (800 - panel_size) / 2;
  int panel_y = (800 - panel_size) / 2;
  int sprite_size = 150;
  int padding = (panel_size - sprite_size * 2) / 3;

  SDL_FRect panel = {panel_x, panel_y, panel_size, panel_size};
  SDL_SetRenderDrawColor(game->renderer, 26, 26, 26, 255);
  SDL_RenderFillRect(game->renderer, &panel);
  SDL_SetRenderDrawColor(game->renderer, 220, 220, 220, 255);
  SDL_RenderRect(game->renderer, &panel);

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

  for (int i = 0; i < 4; i++) {
    SDL_Texture *t = get_texture(game, types[i], game->selected_piece->color);
    SDL_RenderTexture(game->renderer, t, NULL, &spots[i]);
  }
}

void draw_game_over(Game *game) {
  // dark transparent overlay
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 150);
  SDL_FRect overlay = {0, 0, 800, 800};
  SDL_RenderFillRect(game->renderer, &overlay);

  // panel
  int panel_size = 300;
  int panel_x = (800 - panel_size) / 2;
  int panel_y = (800 - panel_size) / 2;
  SDL_FRect panel = {panel_x, panel_y, panel_size, panel_size};
  SDL_SetRenderDrawColor(game->renderer, 26, 26, 26, 255);
  SDL_RenderFillRect(game->renderer, &panel);
  SDL_SetRenderDrawColor(game->renderer, 220, 220, 220, 255);
  SDL_RenderRect(game->renderer, &panel);

  // winner king sprite centered in panel

  if (game->won != NO_COLOR) {
    SDL_Texture *winner = get_texture(game, KING, game->won);
    SDL_Texture *loser =
        get_texture(game, KING, game->won == WHITE ? BLACK : WHITE);
    // winner king larger in center
    SDL_FRect wr = {panel_x + 75, panel_y + 75, 150, 150};
    // loser king smaller, bottom corner, slightly faded
    SDL_FRect lr = {panel_x + 190, panel_y + 190, 80, 80};
    SDL_RenderTexture(game->renderer, winner, NULL, &wr);
    SDL_SetTextureAlphaMod(loser, 100);
    SDL_RenderTexture(game->renderer, loser, NULL, &lr);
    SDL_SetTextureAlphaMod(loser, 255); // reset alpha
  } else if (game->stalemate) {

    SDL_Texture *wk = get_texture(game, KING, WHITE);
    SDL_Texture *bk = get_texture(game, KING, BLACK);
    SDL_SetTextureAlphaMod(wk, 180);
    SDL_SetTextureAlphaMod(bk, 180);
    SDL_FRect wr = {panel_x + 40, panel_y + 75, 100, 100};
    SDL_FRect br = {panel_x + 160, panel_y + 75, 100, 100};
    SDL_RenderTexture(game->renderer, wk, NULL, &wr);
    SDL_RenderTexture(game->renderer, bk, NULL, &br);
    SDL_SetTextureAlphaMod(wk, 255);
    SDL_SetTextureAlphaMod(bk, 255);
  }
};

bool draw(Game *game) {
  switch (game->current_scene) {

  case PROMOTING:
    draw_board(game->renderer);
    draw_pieces(game);
    draw_promotion_menu(game);
    break;
  case PLAYING:
    draw_board(game->renderer);
    draw_pieces(game);
    draw_hints(game);
    break;
  case GAME_OVER:
    draw_board(game->renderer);
    draw_pieces(game);
    draw_game_over(game);
    break;
  case MENU:
    break;
  }

  return true;
}

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename);

void init_board(Game *game) {
  /* Clear board */
  for (int i = 0; i < BOARD_CELLS; i++) {
    game->board[i] = (Piece){.x = i % BOARD_SIZE,
                             .y = i / BOARD_SIZE,
                             .sprite = NULL,
                             .type = NO_TYPE,
                             .color = NO_COLOR,
                             .isSelected = false,
                             .isAlive = false,
                             .moved = false,
                             .can_enpassant = false};
  }

  /* Load textures */

  game->textures[BLACK][PAWN] =
      load_texture(game->renderer, "sprites/pawn-b.bmp");
  game->textures[WHITE][PAWN] =
      load_texture(game->renderer, "sprites/pawn-w.bmp");
  game->textures[BLACK][ROOK] =
      load_texture(game->renderer, "sprites/rook-b.bmp");
  game->textures[WHITE][ROOK] =
      load_texture(game->renderer, "sprites/rook-w.bmp");
  game->textures[BLACK][KNIGHT] =
      load_texture(game->renderer, "sprites/knight-b.bmp");
  game->textures[WHITE][KNIGHT] =
      load_texture(game->renderer, "sprites/knight-w.bmp");
  game->textures[BLACK][BISHOP] =
      load_texture(game->renderer, "sprites/bishop-b.bmp");
  game->textures[WHITE][BISHOP] =
      load_texture(game->renderer, "sprites/bishop-w.bmp");
  game->textures[BLACK][QUEEN] =
      load_texture(game->renderer, "sprites/queen-b.bmp");
  game->textures[WHITE][QUEEN] =
      load_texture(game->renderer, "sprites/queen-w.bmp");
  game->textures[BLACK][KING] =
      load_texture(game->renderer, "sprites/king-b.bmp");
  game->textures[WHITE][KING] =
      load_texture(game->renderer, "sprites/king-w.bmp");

  PiecesType back_rank[BOARD_SIZE] = {ROOK, KNIGHT, BISHOP, QUEEN,
                                      KING, BISHOP, KNIGHT, ROOK};

  for (int i = 0; i < BOARD_SIZE; i++) {
    /* Black back rank */
    int idx = i;
    game->board[idx].type = back_rank[i];
    game->board[idx].color = BLACK;
    game->board[idx].sprite = game->textures[BLACK][back_rank[i]];
    game->board[idx].isAlive = true;

    /* Black pawns */
    idx = i + 8;
    game->board[idx].type = PAWN;
    game->board[idx].color = BLACK;
    game->board[idx].sprite = game->textures[BLACK][PAWN];
    game->board[idx].isAlive = true;

    /* White pawns */
    idx = i + 48;
    game->board[idx].type = PAWN;
    game->board[idx].color = WHITE;
    game->board[idx].sprite = game->textures[WHITE][PAWN];
    game->board[idx].isAlive = true;

    /* White back rank */
    idx = i + 56;
    game->board[idx].type = back_rank[i];
    game->board[idx].color = WHITE;
    game->board[idx].sprite = game->textures[WHITE][back_rank[i]];
    game->board[idx].isAlive = true;
  }
}

static SDL_Texture *load_texture(SDL_Renderer *renderer, const char *filename) {
  SDL_Surface *surface = SDL_LoadBMP(filename);
  if (!surface) {
    printf("Failed to load surface: %s\n", SDL_GetError());
    return NULL;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_DestroySurface(surface);

  if (!texture)
    printf("Failed to create texture: %s\n", SDL_GetError());

  return texture;
}

void draw_pieces(Game *game) {
  for (int i = 0; i < BOARD_CELLS; i++) {
    if (!game->board[i].isAlive)
      continue;

    SDL_FRect rect = {.x = game->board[i].x * SQUARE_SIZE,
                      .y = game->board[i].y * SQUARE_SIZE,
                      .w = SQUARE_SIZE,
                      .h = SQUARE_SIZE};

    SDL_RenderTexture(game->renderer, game->board[i].sprite, NULL, &rect);
  }
}
