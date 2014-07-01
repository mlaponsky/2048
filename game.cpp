#include <iostream>
#include <gtk/gtk.h>
#include "game.hpp"

using namespace std;

//Maps coordinates to a linear id number 0-15.
int coord_map(int row, int col) {
	return 4*row + col;
};

//Generates the new tile value: produces 2 90% of the time, 4 10%.
int new_tile_val() {
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distr(0.0,1.0);
	double test = distr(generator);
	if (test < 0.9) {
		return 2;
	} else {
		return 4;
	}
};


//Randomly generates a location for tile. If the tile is already filled, calls recursively.
//Only calls new_tile_val once, so as not to skew frequencies of tile values.
void fill_new_tile(Tile_t *board[4][4]) {
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distr(0,15);
	int id_num = distr(generator);

	for (int i = ROW_MIN; i < ROW_MAX; i++) {
		for (int j = COL_MIN; j < COL_MAX; j++) {
			if ((board[i][j]->id_num == id_num) && (board[i][j]->val == 0)) {
				board[i][j]->val = new_tile_val();
			} else {
				fill_new_tile(board);
			}
		}
	}
};


//Initializes the board values. Assigns coords and id numbers while walking through array.
//Initially sets all values to 0, and all tile pointers to NULL.
void mk_board(Tile_t *board[4][4]) {
	for (int row = ROW_MIN; row < ROW_MAX; row++) {
		for (int col = COL_MIN; col < COL_MAX; col++) {
			Tile_t *tile = board[row][col];
			tile->val = 0;
			tile->row = row;
			tile->col = col;
			tile->id_num = coord_map(row, col);
			if (row == ROW_MIN) {
				tile->up = NULL;
			} else {
				tile->up = board[row-1][col];
			}

			if (row == ROW_MAX) {
				tile->down = NULL;
			} else {
				tile->down = board[row+1][col];
			}

			if (col == COL_MIN) {
				tile->left = NULL;
			} else {
				tile->left = board[row][col-1];
			}

			if (col == COL_MAX) {
				tile->right = NULL;
			} else {
				tile->right = board[row][col+1];
			}
		}
	}
};

//Shifts all tiles in a row in a given allowed direction.
void shift_row(Tile_t *board[4][4], int row, int direction) {
	if (direction == LEFT) {
		Tile_t *tile = board[row][COL_MIN];
		while (tile->row != COL_MAX) {
			if (tile->val == 0) {
				while ((tile->right != NULL) && (tile->val == 0)) {
					tile->val = tile->right->val;
					tile->right->val = 0;
					tile->right = tile->right->right;
				}
			}
			tile->right	= board[row][(tile->col)+1];
			tile = tile->right;
		}
	}

	if (direction == RIGHT) {
		Tile_t *tile = board[row][COL_MAX];
		while (tile->col != COL_MIN) {
			if (tile->val == 0) {
				while ((tile->left != NULL) && (tile->val == 0)) {
					tile->val = tile->left->val;
					tile->left->val = 0;
					tile->left = tile->left->left;
				}
			}
			tile->left	= board[row][(tile->col)-1];
			tile = tile->left;
		}
	}
};

//Shifts all tiles in a column in the given direction.
void shift_col(Tile_t *board[4][4], int col, int direction) {
	if (direction == UP) {
		Tile_t *tile = board[ROW_MIN][col];
		while (tile->row != ROW_MAX) {
			if (tile->val == 0) {
				while ((tile->down != NULL) && (tile->val == 0)) {
					tile->val = tile->down->val;
					tile->down->val = 0;
					tile->down = tile->down->down;
				}
			}
			tile->down= board[(tile->row)+1][col];
			tile = tile->down;
		}
	}

	if (direction == DOWN) {
		Tile_t *tile = board[ROW_MAX][col];
		while (tile->row != ROW_MIN) {
			if (tile->val == 0) {
				while ((tile->up != NULL) && (tile->val == 0)) {
					tile->val = tile->up->val;
					tile->up->val = 0;
					tile->up = tile->up->up;
				}
			}
			tile->up	= board[(tile->row)-1][col];
			tile = tile->up;
		}
	}
};

//Combines row values if consecutive nonzero values in a given row and direction are equal.
void combine_row(Tile_t *board[4][4], int row, int direction) {
	if (direction == LEFT) {
		Tile_t *tile = board[row][COL_MIN];
		while (tile->col != COL_MAX) {
			if (tile->right->val == 0) {
				while (tile->right != NULL) {
					if (tile->right->val == 0) {
						tile->right = tile->right->right;
					}
				}
			}
			if (tile->val == tile->right->val) {
				tile->val = tile->val + tile->right->val;
				tile->right->val = 0;
			}
			tile->right = board[row][(tile->col)+1];
			tile = tile->right;
		}
	}

	if (direction == RIGHT) {
		Tile_t *tile = board[row][COL_MAX];
		while (tile->col != COL_MIN) {
			if (tile->left->val == 0) {
				while (tile->left != NULL) {
					if (tile->left->val == 0) {
						tile->left = tile->left->left;
					}
				}
			}
			if (tile->val == tile->left->val) {
				tile->val = tile->val + tile->left->val;
				tile->left->val = 0;
			}
			tile->left = board[row][(tile->col)-1];
			tile = tile->left;
		}
	}
};

//Combined column values if consecutive nonzero values in a given column and direction are equal.
void combine_col(Tile_t *board[4][4], int col, int direction) {
	if (direction == UP) {
		Tile_t *tile = board[ROW_MIN][col];
		while (tile->row != ROW_MAX) {
			if (tile->down->val == 0) {
				while (tile->down != NULL) {
					if (tile->down->val == 0) {
						tile->down = tile->down->down;
					}
				}
			}
			if (tile->val == tile->down->val) {
				tile->val = tile->val + tile->down->val;
				tile->down->val = 0;
			}
			tile->down = board[(tile->row)+1][col];
			tile = tile->down;
		}
	}

	if (direction == DOWN) {
		Tile_t *tile = board[ROW_MAX][col];
		while (tile->row != ROW_MIN) {
			if (tile->up->val == 0) {
				while (tile->up != NULL) {
					if (tile->up->val == 0) {
						tile->up = tile->up->up;
					}
				}
			}
			if (tile->val == tile->up->val) {
				tile->val = tile->val + tile->up->val;
				tile->up->val = 0;
			}
			tile->up = board[(tile->row)-1][col];
			tile = tile->up;
		}
	}
};

//Executes a single move on the board given the move direction.
void mv_board(Tile_t *board[4][4], int direction) {
	if (direction % 2 != 0) {
		for (int row = ROW_MIN; row < ROW_MAX; row++) {
			shift_row(board, row, direction);
			combine_row(board, row, direction);
			shift_row(board, row, direction);
		}
	} else {
		for (int col = COL_MIN; col < COL_MAX; col++) {
			shift_col(board, col, direction);
			combine_col(board, col, direction);
			shift_col(board, col, direction);
		}
	}
};

int main() {
	Tile_t *board[4][4] = tiles;
	mk_board(board);

	bool win = false;
	while (!win) {

	}
}
