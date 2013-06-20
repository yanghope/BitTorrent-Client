// pwp_message_header.h
//
// Defines PWP message definitions.
#ifndef _PWP_MESSAGE_HEADER_H
#define _PWP_MESSAGE_HEADER_H

#ifndef BT_PROTOCOL
#define BT_PROTOCOL "BitTorrent protocol"
#endif

#include <pthread.h>

#define little_to_big_endian(p,a) (a) = ntohl(*((uint32_t*)(p)))
#define big_to_little_endian(a,p) *((uint32_t*)(p)) = htonl((a))

typedef struct pwp_arg {
  int socket_fd;
  char id[21]; // 20 characters   + null-terminated.
  char ip[16]; // 123.567.901.345 + null-terminated.
  int port;

  int connecting; // 1 if connecting, 0 otherwise (accepting).
} pwp_arg_t;

typedef struct pwp_handshake_message {
  unsigned char name_length; // 19
  char protocol_name[19];    // "BitTorrent protocol" (defined as BT_PROTOCOL in btdata.h)
  char reserved[8];          // (nothing)
  int  info_hash[5];         // SHA-1 hash of the info key in metainfo file.
  char peer_id[20];          // self-designated peer ID
} pwp_handshake_msg_t;       // Total 68 bytes (defined as HANDSHAKE_LEN in btdata.h)

typedef struct pwp_message {
  int   message_length;
  char  message_id;
  char* payload;
} pwp_msg_t;

// If this variable is 1, then pwp_handshake_msg is already initialized.
int g_pwp_handshake_init;// = 0;
pthread_mutex_t g_pwp_handshake_init_lock;// = PTHREAD_MUTEX_INITIALIZER;
pwp_handshake_msg_t g_pwp_handshake_msg;

// Message types given in IDs.
#define CHOKE        0
#define UNCHOKE      1
#define INTERESTED   2
#define UNINTERESTED 3
#define HAVE         4
#define BITFIELD     5
#define REQUEST      6
#define PIECE        7
#define CANCEL       8
#define KEEP_ALIVE   10
#define NO_MESSAGE_SENT 100

typedef struct pwp_request {
	int index;
	int offset;
	int length;
} pwp_request_t;

// Each connection manager thread maintains this structure to store information
// about itself and the peer.
typedef struct pwp_state {
  int socket_fd;
  int pwp_index;

  int am_choking;
  int am_interested;
  int am_choked;
  int am_interesting;

  int was_choking;
  int was_interested;
  int was_choked;
  int was_interesting;

  unsigned long int peer_last_heard;
  unsigned long int me_last_talked;

  unsigned char* my_bitfield;
  unsigned char* peer_bitfield;
  unsigned char* diff_bitfield;

  char recv_msg_id;
  int recv_msg_size;
  char* recv_msg_payload;

  int piece_len;
  int current_piece_index;
  int block_index;
  void* queue_requests_from_peer;
  pwp_request_t request_to_peer;
  char* piece_in_progress;
} pwp_state_t;

#endif
