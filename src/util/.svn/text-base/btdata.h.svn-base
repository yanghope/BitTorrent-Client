#ifndef BTDATA_H
#define BTDATA_H

#include <pthread.h>
#include "bencode.h"
#include "../pwp/pwp_message_header.h"

/**************************************
 * Usefule Quantities
**************************************/
/*
#define HANDSHAKE_LEN 68  // Length of peer handshake string in bytes
#define BT_PROTOCOL "BitTorrent protocol"
#define INFOHASH_LEN 20
#define PEER_ID_LEN 20
#define MAXPEERS 100
*/

#ifndef KEEP_ALIVE_INTERVAL
#define KEEP_ALIVE_INTERVAL 12
#endif

#define BT_STARTED 0
#define BT_STOPPED 1
#define BT_COMPLETED 2

/**************************************
 * Data structures
**************************************/
// Data portion of tracker HTTP response
typedef struct _tracker_response {
  int size;       // Number of bytes of bencoded string
  char* data;     // Bencoded string
} tracker_response;

// Data contained in a metafile
typedef struct _torrentmetadata {
  int info_hash[5]; // info_hash for the torrent (SHA1 hash of info key)
  char* announce; // URL of the tracker
  int length;     // file length in bytes
  char* name;     // name of file
  int piece_len;  // number of bytes in a piece
  int num_pieces; // number of pieces (for convenience)
  char* pieces;   // string of concatenated 20-byte SHA1 hash values for all pieces
} torrentmetadata_t;

// Data contained in announce url (i.e. host name and port)
typedef struct _announce_url_t {
  char* hostname;
  int port;
} announce_url_t;

// Data fields for peers returned by tracker
typedef struct _peerdata {
  char id[21]; // 21 for null-terminator
  int port;
  char* ip; // Null terminated
} peerdata;

// Data contained in the tracker response
typedef struct _tracker_data {
  int interval;
  int numpeers;
  peerdata* peers;
} tracker_data;

typedef struct _tracker_request {
  int info_hash[5];
  char peer_id[20];
  int port;
  int uploaded;
  int downloaded;
  int left;
  char ip[16]; // ip addr of myself in XXX.XXX.XXX.XXX form with room for '\0'
} tracker_request;

// This maintains data for an established connection to another peer
typedef struct _peer_t {
  int sockfd;
  int am_choking;        // As uploader, chokes remote peer
  int am_interested;     // Remote peer is interested in our pieces
  int is_choking;        // As downloader, we're choked by remote peer
  int is_interested;     // As downloader, interested in piece of remote peer
  char name[20]; 
} peer_t;

/**************************************
 * Global variables 
**************************************/
char g_my_ip[128]; // Should be in XXX.XXX.XXX.XXX form, but have extra space just in case, null terminated
int g_peerport; // Port we listen on for other peer connections
int g_peerportnum; // Port number g_peerport is bound to
int g_listening;  // Indicates if listening thread has set up its port yet (lock unnecessary)
int g_infohash[5]; // SHA1 hash of the file this client is working on, yes each client will only handle 1
char g_my_id[20];

int g_done; // Indicates whether program should terminate

torrentmetadata_t* g_torrentmeta;
char* g_filedata;      // Actual bytes of the file
int g_filelen;
int g_num_pieces;
char *g_filename;

char g_tracker_ip[16]; // IP of tracker in XXX.XXX.XXX.XXX form (null-terminated)
int g_tracker_port;
tracker_data *g_tracker_response;
pthread_mutex_t g_tracker_response_mutex;

// These variables show up in make_tracker_request (and need to be updated
// during execution).
int g_uploaded;
int g_downloaded;
int g_left;
pthread_mutex_t g_tracker_report_variables_mutex;

// In bitfield, each bit represents a piece.
// Bit set as 0 represents a piece we don't have.
// Bit set as 1 represents a piece we have.
unsigned char* g_bitfield;
int g_bitfield_size;
pthread_mutex_t g_bitfield_mutex;

// List of IP address / port number pairs I am currently connected to.
typedef struct remote_peer {
  char id[21];
  int port;
  char* ip;
  struct remote_peer *next;
} remote_peer_t;
remote_peer_t* g_current_connections;

#define MAX_PWP_CONNECTIONS 100
pwp_arg_t g_pwp_connections[MAX_PWP_CONNECTIONS];
pthread_mutex_t g_pwp_connections_mutex;

int g_current_connections_number;

#define BLOCK_SIZE 8192
// 2^13 = 8192
// 2^14 = 16384
// 2^15 = 32768

#endif
