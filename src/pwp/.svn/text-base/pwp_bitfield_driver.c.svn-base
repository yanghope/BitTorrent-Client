// pwp_bitfield.c
//
// CS 60 -- Project -- Team2
//
// Defines bitfield manipulation functions.

// Includes a driver for testing purposes (temporary).

// JHL. Notes.
//
// 1. Write comments.
// 2. bitfield_from_file: where is the constant for the size of each file piece?
// 3. bitfield_from_file: where is the mutex for reading and writing from file?

#include "./../util/jhl_lib.h"

// Arbitrary values for testing.
#define FILE_BLOCK_SIZE 4
#define FILE_TOTAL_SIZE 65
char g_file[FILE_TOTAL_SIZE] = 
  {
    'a',  'b',  'c',  'd',
    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',

    'e',  'f',  'g',  'h',
    '\0', '\0', '\0', '\0',
    'i',  'j',  'k',  'l',
    'm',  'n',  'o',  'p',

    'i',  'j',  'k',  'l',
    'm',  'n',  'o',  'p',
    'i',  'j',  'k',  'l',
    'm',  'n',  'o',  'p',

    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',
    '\0', 'q', '\0', 'r',
    '\0', '\0', '\0', 's',

    '\0'
  };

char peer_file[FILE_TOTAL_SIZE] =
  {
    'a',  'b',  'c',  'd',
    '\0', '\0', '\0', '\0',
    '1',  '2',  '3',  '4',
    '\0', '\0', '\0', '\0',

    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',
    'm',  'n',  'o',  'p',

    'i',  'j',  'k',  'l',
    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',
    '\0', '\0', '\0', '\0',

    '\0', '\0', '\0', '\0',
    'a',  'a',  'a',  'a',
    '\0', 'q',  '\0', 'r',
    '\0', '\0', '\0', 's',

    '!'
  };

// Each bit represents 1 piece.
// So each unsigned char represents 8 pieces.
// So if there are n pieces, there will be roundup(n/8) pieces.
unsigned char* bitfield_create(int num_pieces) {
  int bitfield_size = num_pieces / 8 + !!(num_pieces % 8);
  unsigned char* bitfield = (unsigned char*) jhl_malloc(bitfield_size * sizeof(unsigned char));
  return bitfield;
}
// return (unsigned char*) jhl_malloc((num_pieces / 8 + !!(num_pieces % 8)) * sizeof(unsigned char));

// Marks that we have the nth (piece_index) piece.
void bitfield_set(unsigned char* bitfield, int piece_index) {
  int field_index = piece_index / 8;
  int bit_index   = piece_index % 8;
  bitfield[field_index] |= (1 << (7 - bit_index));
}

// Marks that we don't have the nth (piece_index) piece.
void bitfield_clear(unsigned char* bitfield, int piece_index) {
  int field_index = piece_index / 8;
  int bit_index   = piece_index % 8;
  bitfield[field_index] &= ~(1 << (7 - bit_index));
}

// Checks if a file has piece in this block.
int bitfield_file_has_block(char* ptr, int block_size) {
  for(int i = 0; i < block_size; i++) {
    if( *(ptr + i) != '\0' ) return 1;
  }
  return 0;
}

//                          Probably global variable.
//                         (But it was easier testing)
//                                    |
//                                    |
//                                    v
unsigned char* bitfield_from_file(char* file) {
  // Obtain these values from relevant global variables.
  // Change the values for actual use.
  // char* file = g_filedata;        // Pointer to the file (g_filedata in btdata.h).
  int   block_size = FILE_BLOCK_SIZE; // Size of each block (?).
  int   file_size  = FILE_TOTAL_SIZE; // Size of the file   (g_file_len in btdata.h).

  int num_pieces = file_size / block_size + !!(file_size % block_size);
  unsigned char* bitfield = bitfield_create(num_pieces);

  // Last block size is file_size % block_size, but if file_size is a multiple of block_size,
  // Then the last block is going to be as large as block_size.
  // (A block's size shall be a positive integer.)
  int last_block_size = file_size % block_size;
  if( last_block_size == 0 ) last_block_size = block_size;

  int retval = 0;

  // pthread_mutex_lock(&file_mutex);
  // Block[0] to Block[n-2];
  int i = 0;
  for( ; i < num_pieces - 1; i++) {
    retval = bitfield_file_has_block(file + (i * block_size), block_size);
    if( retval ) {
      bitfield_set(bitfield, i);
    }
  }
  // Block[n-1]--the last block.
  retval = bitfield_file_has_block(file + (i *block_size), last_block_size);
  if( retval ) {
    bitfield_set(bitfield, i);
  }
  // pthread_mutex_unlock(&file_mutex);

  return bitfield;
}

unsigned char* bitfield_compare(unsigned char* my_bitfield, unsigned char* peer_bitfield, int bitfield_size) {
  unsigned char* bitfield_obtainable = (unsigned char*) jhl_malloc(bitfield_size * sizeof(char));

  for(int i = 0; i < bitfield_size; i++) {
    // Takes union of:     (what I don't have)   (what peer has)
    bitfield_obtainable[i] = ~my_bitfield[i]  &  peer_bitfield[i];
  }

  return bitfield_obtainable;
}

void bitfield_print_bit(unsigned char bit) {
  for(int i = 0; i < 8; i++) {
    if( bit >= 128 ) printf("1");
    else             printf("0");
    bit = bit << 1;
  }
  printf(" ");
}

void bitfield_print(unsigned char* bitfield, int bitfield_size) {
  printf("bitfield:");
  for(int i = 0; i < bitfield_size; i++) {
    bitfield_print_bit(bitfield[i]);
  }
  printf("\n");
}

int bitfield_interested(unsigned char* bitfield_obtainable, int bitfield_size) {
  for(int i = 0; i < bitfield_size; i++) {
    if( *bitfield_obtainable > 0 ) {
      return 1;
    }
  }
  return 0;
}

// Argument 1 is bitfield_obtainable: bitfield where 1 indicates a piece which peer has but I don't,
//                                               and 0 indicates a piece in which I'm not interested.
// Given this argument, returns a index for a piece in which I'm interested.
// Among the interested pieces, decides a random index.
int bitfield_obtainable_random(unsigned char* bitfield_obtainable, int bitfield_size) {
  unsigned char bit;
  int count = 0;
  int count_index = -1;  // Index among counted obtainable pieces.  In 00110000, the two 1s are 0 and 1.
                         // Return value is index among all pieces. In 00110000, the two 1s are 2 and 3.
                         //                                         So we will return either 2 or 3.
                         //                                         Not 0 or 1.

  // First, count the number of obtainable pieces.
  for(int i = 0; i < bitfield_size; i++) {
    bit = bitfield_obtainable[i];
    for(int j = 0; j < 8; j++) {
      if( bit >= 128 ) count++;
      bit = bit << 1;
    }
  }

  if( count == 0 ) {
    return -1;
  }

  // Then determine a pseudo-random count index.
  count_index = rand() % count;
  //if( JHL_LOG ) printf("count = %d, count_index = %d\n", count, count_index);
  // count_index-th obtainable piece will be requested.

  // Determine which piece is this random piece.
  count = 0;
  for(int i = 0; i < bitfield_size; i++) {
    bit = bitfield_obtainable[i];
    for(int j = 0; j < 8; j++) {
      if( bit >= 128 ) {
	count++;
	if( count - 1  == count_index ) {
	  return i * 8 + j;
	}
      }
      bit = bit << 1;
    }
  }

  // Code should not reach here.
  jhl_log("pwp$bitfield$bitfield_obtainable_random: code reaches where it shouldn't");
  return -2;
}

// Driver for testing purposes.
int main(int argc, char** argv) {
  jhl_log("bitfield driver running...");

  int num_pieces = FILE_TOTAL_SIZE / FILE_BLOCK_SIZE + !!(FILE_TOTAL_SIZE % FILE_BLOCK_SIZE);
  int size = num_pieces / 8 + !!(num_pieces % 8);

  unsigned char* bitfield;
  unsigned char* peer_bitfield;
  unsigned char* bitfield_obtainable;

  bitfield = bitfield_from_file(g_file);
  peer_bitfield = bitfield_from_file(peer_file);
  bitfield_obtainable = bitfield_compare(bitfield, peer_bitfield, size);

  printf("mine: "); bitfield_print(bitfield, size);
  printf("peer: "); bitfield_print(peer_bitfield, size);
  printf("obtn: "); bitfield_print(bitfield_obtainable, size);

  for(int i = 0; i < 10; i++) {
    printf("Random piece: %d\n", bitfield_obtainable_random(bitfield_obtainable, size));
  }

  jhl_free(bitfield);
  jhl_log("bitfield driver exiting...");
  exit(0);
}
