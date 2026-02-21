#ifndef RENDERER_H
#define RENDERER_H

#include "game.h"

SDL_Texture *get_texture(Game *game, PiecesType type, PieceColor color);
void draw_pieces(Game *game);
void init_board(Game *game);
bool draw(Game *game);

#endif // !RENDERER_H
