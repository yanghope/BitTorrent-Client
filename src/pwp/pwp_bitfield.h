#ifndef _PWP_BITFIELD_C
#define _PWP_BITFIELD_C

#include "./../util/jhl_lib.h"
#include "./../util/btdata.h"
#include "./../util/sha1.h"

void bitf_set_bit(unsigned char* bitfield, int piece_index);

void bitf_clear_bit(unsigned char* bitfield, int piece_index);

void bitf_print_bit(unsigned char bit);

void bitf_print(unsigned char* bitfield, int bitfield_size);

unsigned char* bitf_create(int bitfield_size);

int bitf_check_file_block(char* ptr, int piece_index, int block_size);

void bitf_global_init();

void bitf_find_obtainable(unsigned char* obtainable, unsigned char* peer);

int bitf_is_interesting(unsigned char* bitfield_obtainable);

int bitf_obtainable_random(unsigned char* bitfield_obtainable);

int bitf_find_unknown_to_peer(unsigned char* my_bitfield);

float bitf_calculate_percentage(unsigned char* bitfield);

#endif
