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

bool draw(Game *game) {
  draw_board(game->renderer);
  draw_pieces(game);
  draw_hints(game);
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
  SDL_Texture *textures[2][6];

  textures[BLACK][PAWN] = load_texture(game->renderer, "../sprites/pawn-b.bmp");
  textures[WHITE][PAWN] = load_texture(game->renderer, "../sprites/pawn-w.bmp");
  textures[BLACK][ROOK] = load_texture(game->renderer, "../sprites/rook-b.bmp");
  textures[WHITE][ROOK] = load_texture(game->renderer, "../sprites/rook-w.bmp");
  textures[BLACK][KNIGHT] =
      load_texture(game->renderer, "../sprites/knight-b.bmp");
  textures[WHITE][KNIGHT] =
      load_texture(game->renderer, "../sprites/knight-w.bmp");
  textures[BLACK][BISHOP] =
      load_texture(game->renderer, "../sprites/bishop-b.bmp");
  textures[WHITE][BISHOP] =
      load_texture(game->renderer, "../sprites/bishop-w.bmp");
  textures[BLACK][QUEEN] =
      load_texture(game->renderer, "../sprites/queen-b.bmp");
  textures[WHITE][QUEEN] =
      load_texture(game->renderer, "../sprites/queen-w.bmp");
  textures[BLACK][KING] = load_texture(game->renderer, "../sprites/king-b.bmp");
  textures[WHITE][KING] = load_texture(game->renderer, "../sprites/king-w.bmp");

  PiecesType back_rank[BOARD_SIZE] = {ROOK, KNIGHT, BISHOP, QUEEN,
                                      KING, BISHOP, KNIGHT, ROOK};

  for (int i = 0; i < BOARD_SIZE; i++) {
    /* Black back rank */
    int idx = i;
    game->board[idx].type = back_rank[i];
    game->board[idx].color = BLACK;
    game->board[idx].sprite = textures[BLACK][back_rank[i]];
    game->board[idx].isAlive = true;

    /* Black pawns */
    idx = i + 8;
    game->board[idx].type = PAWN;
    game->board[idx].color = BLACK;
    game->board[idx].sprite = textures[BLACK][PAWN];
    game->board[idx].isAlive = true;

    /* White pawns */
    idx = i + 48;
    game->board[idx].type = PAWN;
    game->board[idx].color = WHITE;
    game->board[idx].sprite = textures[WHITE][PAWN];
    game->board[idx].isAlive = true;

    /* White back rank */
    idx = i + 56;
    game->board[idx].type = back_rank[i];
    game->board[idx].color = WHITE;
    game->board[idx].sprite = textures[WHITE][back_rank[i]];
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
