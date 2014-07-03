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
201422793957167
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

board_t get_column(board_t board) {
  
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
  if (~tmp == ~tmp | 0xF) {
    tmp |= new_tile_val();
    tmp <<= 4*loc;
    board |= tmp_board;
  } else {
    mk_new_tile(board);      // Recursively call if slot is full.
  }
};


// The lookup tables.
row_t table_left[65536];
row_t table_right[65536];
board_t table_up[65536];
board_t table_down[65536];
