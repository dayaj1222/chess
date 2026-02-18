#include "../include/pieces.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/pieces.h"

// Declaration
SDL_Texture *load_texture(SDL_Renderer *renderer, char *filename);
void draw_piece(Piece *board, SDL_Renderer *renderer);

// Definition
void init_board(Game *game) {
  for (int i = 0; i < 64; i++) {
    game->board[i] = (Piece){
        .x = i % 8,
        .y = i / 8,
        .sprite = NULL,
        .type = NO_TYPE,
        .color = NO_COLOR,
        .isSelected = false,
        .isAlive = false,
        .moved = false,
    };
  }

  SDL_Texture *b_pawn = load_texture(game->renderer, "../sprites/pawn-b.bmp");
  SDL_Texture *w_pawn = load_texture(game->renderer, "../sprites/pawn-w.bmp");
  SDL_Texture *b_rook = load_texture(game->renderer, "../sprites/rook-b.bmp");
  SDL_Texture *w_rook = load_texture(game->renderer, "../sprites/rook-w.bmp");
  SDL_Texture *b_knight =
      load_texture(game->renderer, "../sprites/knight-b.bmp");
  SDL_Texture *w_knight =
      load_texture(game->renderer, "../sprites/knight-w.bmp");
  SDL_Texture *b_bishop =
      load_texture(game->renderer, "../sprites/bishop-b.bmp");
  SDL_Texture *w_bishop =
      load_texture(game->renderer, "../sprites/bishop-w.bmp");
  SDL_Texture *b_queen = load_texture(game->renderer, "../sprites/queen-b.bmp");
  SDL_Texture *w_queen = load_texture(game->renderer, "../sprites/queen-w.bmp");
  SDL_Texture *b_king = load_texture(game->renderer, "../sprites/king-b.bmp");
  SDL_Texture *w_king = load_texture(game->renderer, "../sprites/king-w.bmp");

  PiecesType types[] = {ROOK, KNIGHT, BISHOP, QUEEN,
                        KING, BISHOP, KNIGHT, ROOK};

  SDL_Texture *black_t[] = {b_rook, b_knight, b_bishop, b_queen,
                            b_king, b_bishop, b_knight, b_rook};
  SDL_Texture *white_t[] = {w_rook, w_knight, w_bishop, w_queen,
                            w_king, w_bishop, w_knight, w_rook};

  for (int i = 0; i < 8; i++) {
    // Black Back Rank
    game->board[i].sprite = black_t[i];
    game->board[i].type = types[i];
    game->board[i].color = BLACK;
    game->board[i].y = 0;
    game->board[i].isAlive = true;
    game->board[i].moved = false;
    game->board[i].isSelected = false;

    // Black Pawns
    game->board[i + 8].sprite = b_pawn;
    game->board[i + 8].type = PAWN;
    game->board[i + 8].color = BLACK;
    game->board[i + 8].y = 1;
    game->board[i + 8].isAlive = true;
    game->board[i + 8].moved = false;
    game->board[i].isSelected = false;

    // White Pawnn
    game->board[i + 48].sprite = w_pawn;
    game->board[i + 48].type = PAWN;
    game->board[i + 48].color = WHITE;
    game->board[i + 48].y = 6;
    game->board[i + 48].isAlive = true;
    game->board[i + 48].moved = false;
    game->board[i].isSelected = false;

    // White Back Rank
    game->board[i + 56].sprite = white_t[i];
    game->board[i + 56].type = types[i];
    game->board[i + 56].color = WHITE;
    game->board[i + 56].y = 7;
    game->board[i + 56].isAlive = true;
    game->board[i + 56].moved = false;
    game->board[i].isSelected = false;
  }
}

SDL_Texture *load_texture(SDL_Renderer *renderer, char *filename) {
  SDL_Surface *temp_surface = SDL_LoadBMP(filename);

  if (temp_surface == NULL) {
    printf("SDL Couldnt Load picture as a Surface: %s", SDL_GetError());
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, temp_surface);

  if (texture == NULL) {
    printf("Failed to create Texture from Surface: %s", SDL_GetError());
  }

  SDL_DestroySurface(temp_surface);

  return texture;
}

void draw_pieces(Game *game) {

  for (int i = 0; i < 64; i++) {
    if (game->board[i].sprite != NULL) {
      SDL_FRect piece_rect = {
          .x = game->board[i].x * 100, .y = game->board[i].y * 100, 100, 100};
      SDL_RenderTexture(game->renderer, game->board[i].sprite, NULL,
                        &piece_rect);
    }
  }
}
