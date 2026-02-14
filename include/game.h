#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>

typedef enum PiecesType {
  PAWN,
  ROOK,
  KNIGHT,
  BISHOP,
  QUEEN,
  KING,
  NO_TYPE
} PiecesType;

typedef enum PieceColor { WHITE, BLACK, NO_COLOR } PieceColor;

typedef struct Piece {
  int x;
  int y;
  PieceColor color;
  PiecesType type;
  SDL_Texture *sprite;
} Piece;

typedef enum { MENU, PLAYING, PIECE_SELECTED, GAME_OVER } Scene;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  Piece board[64];
  Scene current_scene;
  int selected_index;
  bool running;
} Game;

#endif // !GAME_H
