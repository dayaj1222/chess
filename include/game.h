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
  bool moved;
  bool isAlive;
  bool isSelected;
  bool can_enpassant;

} Piece;

typedef enum { MENU, PLAYING, GAME_OVER } Scene;
typedef enum MoveType { HAS_ENEMY, HAS_SELF, HAS_NONE } MoveType;

typedef struct Move {
  int x;
  int y;
  MoveType spot;
} Move;

typedef struct {
  // SDL
  SDL_Window *window;
  SDL_Renderer *renderer;

  Piece board[64];

  // Game States
  PieceColor turn;
  Scene current_scene;
  bool pending_click;
  Piece *selected_piece;
  Move **possible_moves;
  int clicked_square_x;
  int clicked_square_y;
  bool running;
} Game;

#endif // !GAME_H
