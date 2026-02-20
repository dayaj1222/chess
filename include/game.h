#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef enum { PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NO_TYPE } PiecesType;

typedef enum { WHITE, BLACK, NO_COLOR } PieceColor;

typedef enum { HAS_ENEMY, HAS_SELF, HAS_NONE } MoveType;

typedef enum { MENU, PLAYING, GAME_OVER } Scene;

typedef struct {
  int x;
  int y;
  MoveType spot;
} Move;

typedef struct {
  int x;
  int y;

  PieceColor color;
  PiecesType type;

  SDL_Texture *sprite;

  bool moved;
  bool isAlive;
  bool isSelected;
  bool can_enpassant;
} Piece;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;

  Piece board[64];

  PieceColor turn;
  Scene current_scene;

  bool running;
  bool pending_click;

  int clicked_square_x;
  int clicked_square_y;

  Piece *selected_piece;
  Move **possible_moves;

  bool is_white_checked;
  bool is_black_checked;
} Game;

#endif
