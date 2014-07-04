#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <bitset>
#include <random>

using namespace std;

typedef uint64_t board_t;
typedef uint16_t row_t;

static const board_t ROW_MASK = 0xFFFFULL;
static const board_t COL_MASK = 0x000F000F000F000FULL;

board_t transpose_board(board_t x) {
  /* We need 7 unique boards derived from the original, each one containing           only the elements that will be shifted by the same amount. The shifts correspond to flippng elements across the diagonal. This function will allow us to abstract our row shifts to alse compute column shifts. */

  uint64_t d   = x & 0xF0000F0000F0000FULL;    // Diagonal
  uint64_t r12 = x & 0x0F0000F0000F0000ULL;    // >> 12
  uint64_t r24 = x & 0x00F0000F00000000ULL;    // >> 24
  uint64_t r36 = x & 0x000F000000000000ULL;    // >> 36
  uint64_t l12 = x & 0x0000F0000F0000F0ULL;    // << 12
  uint64_t l24 = x & 0x00000000F0000F00ULL;    // << 24
  uint64_t l36 = x & 0x000000000000F000ULL;    // << 36

  // Compute the transpose:
  uint64_t x_t = d |(r12>>12) | (r24>>24) | (r36>>36) |
                    (l12<<12) | (l24<<24) | (l36<<36);
  return x_t;
};

// Used for shifting rows to the right.
row_t reverse_row(row_t row) {
  return (row>>12) | (row>>4 & 0x00F0) | (row<<4 & 0x0F00) | (row<<12);
};

board_t get_column(row_t row) {
  board_t col = row;
  return (col | (col<<12ULL) | (col<<24ULL) | (col<<36ULL)) & COL_MASK;
};

//
uint64_t new_tile_val() {
  default_random_engine generator;
  uniform_real_distribution<double> distr(0.0,1.0);
  double test = distr(generator);
  if (test < 0.9) {
    return 1;
  } else {
    return 2;
  }
};

void mk_new_tile(board_t board) {
  default_random_engine generator;
  uniform_int_distribution<int> distr(0,15);    // Randomly select a slot 0-15.
  int loc = distr(generator);

  board_t tmp = board>>(4*loc);      // Shift to location.
  //Flipping tmp makes all 0 slots into 0xF. Checks if slot is empty.
  if (~tmp == (~tmp | 0xF)) {
    tmp |= new_tile_val();
    tmp <<= 4*loc;
    board |= tmp;
  } else {
    mk_new_tile(board);      // Recursively call if slot is full.
  }
};

// The lookup tables.
row_t table_left[65536];
row_t table_right[65536];
board_t table_up[65536];
board_t table_down[65536];

void mk_tables() {
  for (unsigned int row = 0; row < 65536; row++) {
    cout << row << endl;
    unsigned int line[4] = {  (row   ) & 0xF,
                              (row>>4) & 0xF,
                              (row>>8) & 0xF,
                              (row>>12) & 0xF   };


    for (int i = 0; i < 3; i++) {
      int j;
      for (j = i+1; j < 4; j++) {
        if (line[j] != 0) break;
      }
      if (j == 4) break;

      if (line[i] == 0) {
        line[i] = line[j];
        line[j] = 0;
        i--;
      } else if (line[i] == line[j] && line[i] != 0xF) {
        line[i]++;
        line[j] = 0;
      }
    }

    row_t move = line[0] | line[1]<<4 | line[2]<<8 | line[3]<<12;
    row_t rev_move = reverse_row(move);
    unsigned int rev_row = reverse_row(row);

    table_left  [    row] =                row  ^ move;
    table_right [rev_row] =            rev_row  ^ rev_move;
    table_up    [    row] =     get_column(row) ^ get_column(move);
    table_down  [rev_row] = get_column(rev_row) ^ get_column(rev_move);
  }
};

void move_left(board_t board) {
  board_t result = board;

  result ^= board_t(table_left[board>> 0] & ROW_MASK) <<  0;
  result ^= board_t(table_left[board>>16] & ROW_MASK) << 16;
  result ^= board_t(table_left[board>>32] & ROW_MASK) << 32;
  result ^= board_t(table_left[board>>48] & ROW_MASK) << 48;
  return result;
};

void move_right(board_t board) {
  board_t result = board;

  result ^= board_t(table_right[board >>  0] & ROW_MASK) <<  0;
  result ^= board_t(table_right[board >> 16] & ROW_MASK) << 16;
  result ^= board_t(table_right[board >> 32] & ROW_MASK) << 32;
  result ^= board_t(table_right[board >> 48] & ROW_MASK) << 48;
  return result;
};

void move_up(board_t board) {
  board_t result = board;
  board_t trans = transpose_board(board);

  result ^= (table_up[trans >>  0] & ROW_MASK) <<  0;
  result ^= (table_up[trans >> 16] & ROW_MASK) <<  4;
  result ^= (table_up[trans >> 32] & ROW_MASK) <<  8;
  result ^= (table_up[trans >> 48] & ROW_MASK) << 12;
  return result;
};

board_t move_down(board_t board) {
  board_t result = board;
  board_t trans = transpose_board(board);

  result ^= (table_down[trans >>  0] & ROW_MASK) <<  0;
  result ^= (table_down[trans >> 16] & ROW_MASK) <<  4;
  result ^= (table_down[trans >> 32] & ROW_MASK) <<  8;
  result ^= (table_down[trans >> 48] & ROW_MASK) << 12;
  return result;
};

void mk_move(board_t board, int direction) {
  switch(direction) {
    case 0:              // up
      move_up(board);
    case 1:              // down
      move_down(board);
    case 2:              // left
      move_left(board);
    case 3:              // right
      move_right(board);
  }
};
