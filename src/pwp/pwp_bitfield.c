// pwp_bitfield.c
//
// CS 60 -- Project -- Team2
//
// Defines bitfield manipulation functions.

#include "pwp_bitfield.h"

// Marks that we have the nth (piece_index) piece.
void bitf_set_bit(unsigned char* bitfield, int piece_index) {
    int field_index = piece_index / 8;
    int bit_index   = piece_index % 8;
    bitfield[field_index] |= (1 << (7 - bit_index));
}

// Marks that we don't have the nth (piece_index) piece.
void bitf_clear_bit(unsigned char* bitfield, int piece_index) {
    int field_index = piece_index / 8;
    int bit_index   = piece_index % 8;
    bitfield[field_index] &= ~(1 << (7 - bit_index));
}

// Helper method for bitfield_print()
void bitf_print_bit(unsigned char bit) {
    for(int i = 0; i < 8; i++) {
        if( bit >= 128 ) printf("1");
        else             printf("0");
        bit = bit << 1;
    }
    printf(" ");
}

// Useful for reference, no use in complete program.
// Format:
//   00011000 01010011 10000100 etc.
// If bitfield ends mid-bit, the rest is displayed as 0.
void bitf_print(unsigned char* bitfield, int bitfield_size) {
    printf("bitfield:\n");
    for(int i = 0; i < bitfield_size; i++) {
        bitf_print_bit(bitfield[i]);
    }
    printf("\n");
}

// Creates a bitfield of n bytes.
unsigned char* bitf_create(int bitfield_size) {
    //return (unsigned char*) jhl_malloc(bitfield_size * sizeof(unsigned char));
    void* ptr = malloc(bitfield_size);
    if(ptr == NULL) {
        perror("jhl_lib$jhl_malloc$malloc");
        exit(1);
    }
    
    if( bitfield_size ) {
        memset(ptr, '\0', bitfield_size);
    } else {
        memset(ptr, '\0', sizeof(ptr));
    }
    //jhl_malloc_record_add(ptr);
    return (unsigned char *)ptr;
}

// Returns 0 if we don't have the file block.
//         1 if we have the file block.
//
// TO DO: change emptiness check to SHA-1 hash check.
int bitf_check_file_block(char* ptr, int piece_index, int block_size) {
    
    //SHA1Context sha;
    //SHA1Reset(&sha);
    //unsigned char MessageDigest[20];
    //SHA1Input(&sha, (unsigned char*) ptr, block_size);
    //if( !SHA1Result(&sha, MessageDigest) ) {
    //  jhl_log("pwp$bitfield$SHA1-hash failed");
    //  return -100;
    //}
    //if( strncmp((char*) MessageDigest, (char*) g_torrentmeta->pieces + piece_index * 20, block_size) != 0 ) {
    //  return 0;
    //} else {
    //  return -1;
    //}
    
    // Emptiness check (delete).
    for(int i = 0; i < block_size; i++) {
        if( *(ptr + i) != '\0' ) return 1;
    }
    return 0;
}

// Initializes the following global variables:
//   g_bitfield
//   g_bitfield_size
//   g_bitfield_mutex
void bitf_global_init() {
    pthread_mutex_init(&g_bitfield_mutex, NULL);
    
    char* file     = g_filedata;               // Pointer to file.
    int block_size = g_torrentmeta->piece_len; // Size of each file piece.
    int file_size  = g_filelen;                // Size of the file.
    
    // number of pieces is determined by block_size and file_size.
    // size of bitfield is the smallest multiple of 8 equal to or larger than num_pieces / 8.
    int num_pieces = file_size / block_size + !!(file_size % block_size);
    g_bitfield_size = num_pieces / 8 + !!(num_pieces % 8);
    
    g_bitfield = bitf_create(g_bitfield_size);
    
    // Last block size is file_size % block_size, but if file_size is a multiple of block_size,
    // Then the last block is going to be as large as block_size.
    // (A block's size shall be a positive integer.)
    int last_block_size = file_size % block_size;
    if( last_block_size == 0 ) last_block_size = block_size;
    
    int retval = 0;
    
    // pthread_mutex_lock(&file_mutex);
    // Block[0] to Block[n-2];
    int i = 0;
    long count = 0;
    for( ; i < num_pieces - 1; i++) {
        retval = bitf_check_file_block(file + (i * block_size), i, block_size);
        if( retval ) {
            bitf_set_bit(g_bitfield, i);
            count++;
            g_left -= block_size;
        }
    }
    // Block[n-1]--the last block.
    retval = bitf_check_file_block(file + (i *block_size), i,  last_block_size);
    if( retval ) {
        bitf_set_bit(g_bitfield, i);
        count++;
        g_left -= last_block_size;
    }
    
    // pthread_mutex_unlock(&file_mutex);
}

void bitf_find_obtainable(unsigned char* obtainable, unsigned char* peer) {
    pthread_mutex_lock(&g_bitfield_mutex);
    
    for(int i = 0; i < g_bitfield_size; i++) {
        // Union of:   (what I don't have)  (what peer has)
        obtainable[i] = ~g_bitfield[i]   &   peer[i];
    }
    
    pthread_mutex_unlock(&g_bitfield_mutex);
}

// Returns: 1 if argument 1 contains at least one interesting piece
//                                                i.e., one I don't have but peer has.
//          0 otherwise.
int bitf_is_interesting(unsigned char* bitfield_obtainable) {
    for(int i = 0; i < g_bitfield_size; i++) {
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
int bitf_obtainable_random(unsigned char* bitfield_obtainable) {
    unsigned char bit;
    int count = 0;
    int count_index = -1;  // Index among counted obtainable pieces.  In 00110000, the two 1s are 0 and 1.
    // Return value is index among all pieces. In 00110000, the two 1s are 2 and 3.
    // ^(random index)                         So we will return either 2 or 3.
    //                                         Not 0 or 1.
    
    // First, count the number of obtainable pieces.
    for(int i = 0; i < g_bitfield_size; i++) {
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
    for(int i = 0; i < g_bitfield_size; i++) {
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
    //jhl_log("pwp$bitfield$bitf_obtainable_random: code reaches where it shouldn't");
    return -2;
}

// Returns piece index of mine_bitfield which has not been reported to peer yet.
// Returns -1 if no such piece exists.
//
// This is useful for examining conditions for sending HAVE message.
//
// If thread 0 receives piece 24 from peer 0,
// g_bitfield[24] should be set to 1.
// thread 0's bitfield[24] should also be set to 0.
// Now thread 1, in examining conditions for sending HAVE message,
// notices that its bitfield[24] is 0 but g_bitfield[24] is 1
// In this case, return 24.
int bitf_find_unknown_to_peer(unsigned char* my_bitfield) {
    unsigned char byte;
    
    pthread_mutex_lock(&g_bitfield_mutex);
    for(int i = 0; i < g_bitfield_size; i++) {
        if( g_bitfield[i] != my_bitfield[i] ) {
            byte = g_bitfield[i] - my_bitfield[i];
            for(int j = 0; j < 8; j++) {
                if( byte >= 128 ) {
                    pthread_mutex_unlock(&g_bitfield_mutex);
                    return i * 8 + j;
                }
                byte = byte << 1;
            }
        }
    }
    
    pthread_mutex_unlock(&g_bitfield_mutex);
    return -1;
}

float bitf_calculate_percentage(unsigned char* bitfield) {
    unsigned char bit;
    int count = 0;
    for(int i = 0; i < g_bitfield_size; i++) {
        bit = bitfield[i];
        for(int j = 0; j < 8; j++) {
            if( bit >= 128 ) count++;
            bit = bit << 1;
        }
    }
    return count / g_bitfield_size;
}