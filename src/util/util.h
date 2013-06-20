

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "btdata.h"
#include "bencode.h"

#ifndef UTIL_H
#define UTIL_H

#define MAXLINE 4096

int is_bigendian();
int endian_convert (char* array, int size);
// Functions to receive data from a connected socket
int recvline(int fd, char **line);
int recvlinef(int fd, char *format, ...);

// Connect to another machine, returns sockfd
int connect_to_host(char* ip, int port);

// Return the length of a file in bytes
int file_len(FILE* fname);

// Extract data from a torrent file
torrentmetadata_t* parsetorrentfile(char* filename);

// Extract the useful data string from a tracker response
tracker_response* preprocess_tracker_response(int sockfd);

// Extract peer connection information from tracker response
tracker_data* get_tracker_data(char* data, int len);
void get_peers(tracker_data* td, be_node* peer_list); // Helper for above
void get_peer_data(peerdata* peer, be_node* ben_res); // Helper for above

// Make a tracker request and return the string to send to the tracker
char* make_tracker_request(int event, int* mlen);

// Helper functions for dealing with integers received from peers
int reverse_byte_orderi(int i);
int make_big_endian(int i);
int make_host_orderi(int i);

//writes g_filedata to a file at filelocation
void write_to_file(char *filelocation);

// Function called for ctrl-c so everything can shut down nicely
void client_shutdown(int sig);

// Function to extract necessary data from a torrent file
announce_url_t* parse_announce_url(char* announce);

// print_metadata
//
// Summary : Prints human-readable output for metadata.
// Args    : torrentmedatdata_t* ptr
int print_metadata(torrentmetadata_t* m);

remote_peer_t* get_new_peers(tracker_data *tracker_response);
int add_to_list(remote_peer_t **list, remote_peer_t *new_peer);
int remove_from_list(remote_peer_t **list, remote_peer_t *peer);
void free_list(remote_peer_t *list);
int checkID(remote_peer_t *peer_1, remote_peer_t *peer_2);
int dupID(remote_peer_t* global, remote_peer_t* local_peer);

#endif
