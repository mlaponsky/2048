//game.h

#ifndef__GAME_H_INCLUDED__
#define __GAME_H_INCLUDED__

#include <cstdio>
#include <cstdlib>
#include <random>

//Declare constants for board manipulation.
enum {LEFT = -1, RIGHT = 1, UP = -2, DOWN = 2};
enum {ROW_MIN = 0, ROW_MAX = 4, COL_MIN = 0, COL_MAX = 4};

//The board structure: A 4x4 array of tiles, with pointers to tiles in all other directions.
struct Tile_t {
  int val;
  int row;
  int col;
  int id_num;
  Tile_t *left;
  Tile_t *right;
  Tile_t *up;
  Tile_t *down;
  Tile_t *next;
} tiles[4][4];

//Function Prototypes.
int coord_map(int row, int col);
int new_tile_val();
void fill_new_tile(Tile_t *board[4][4]);
void mk_board(Tile_t *board[4][4]);
void shift_row(Tile_t *board[4][4], int row, int direction);
void shift_col(Tile_t *board[4][4], int col, int direction);
void combine_row(Tile_t *board[4][4], int row, int direction);
void combine_col(Tile_t *board[4][4], int col, int direction);
void mv_board(Tile_t *board[4][4], int direction);

#endif
