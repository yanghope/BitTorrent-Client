#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "util.h"


int is_bigendian()
{ 
  const int never_used = 1; 
  return( (*(char*)&never_used) == 0 ); 
}

int endian_convert (char* array, int size) {
  int i, j;
  for (i = 0, j = size - 1; i < j; i++, j--) {  
    char b = array[i];  
    array[i] = array[j];  
    array[j] = b;  
  }
  return 0;
}

int connect_to_host(char* ip, int port)
{
  // Get a socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("Could not create socket");
    return(-1);
  }

  // Fill in sockaddr and connect
  struct sockaddr_in addr;
  memset(&addr,0,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);

//    puts("trying connection in connect_to_host!!!");
  if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("Error connecting to socket");
    return(-1);
  }

  return sockfd;
}

int make_listen_port(int port)
{
  int sockfd;

  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sockfd <0)
  {
    perror("Could not create socket");
    return 0;
  }

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
      perror("Could not bind socket");
      return 0;
  }

  if(listen(sockfd, 20) < 0)
  {
    perror("Error listening on socket");
    return 0;
  }

  return sockfd;
}


// Compute the number of bytes of an open file
int file_len(FILE* fp)
{
  int sz;
  fseek(fp , 0 , SEEK_END);
  sz = ftell(fp);
  rewind (fp);
  return sz;
}
/*
int file_len(FILE* fp)
{
  printf("IN FILE_LEN\n");
  int count = 0;
  while( fp && !feof( fp ) )
  {
    unsigned char c;
    fread( &c , sizeof(char), 1, fp );
    printf("%c ",c);
    count++;
  }
  if( fp )
  { 
    rewind( fp ); 
  }
  return count;
}
*/

// recvline(int fd, char **line)
// Description: Retrieve string from socket fd (could be any file descriptor
//              though)
// Input: int of file descriptor, char** that the line will be written into
// Output: int of number of bytes read
int recvline(int fd, char **line)
{
  int retVal;
  int lineIndex = 0;
  int lineSize  = 128;
  
  *line = (char *)malloc(sizeof(char) * lineSize);
  
  if (*line == NULL)
  {
    perror("malloc");
    return -1;
  }
  
  while ((retVal = read(fd, *line + lineIndex, 1)) == 1)
  {
    if ('\n' == (*line)[lineIndex])
    {
      (*line)[lineIndex] = 0;
      break;
    }
    
    lineIndex += 1;
    
    /*
      Reallocate line buffer if we get too many characters.
    */
    if (lineIndex > lineSize)
    {
      lineSize *= 2;
      char *newLine = realloc(*line, sizeof(char) * lineSize);
      
      if (newLine == NULL)
      {
        retVal    = -1; /* realloc failed */
        break;
      }
      
      *line = newLine;
    }
  }
  
  if (retVal < 0)
  {
    free(*line);
    return -1;
  }
  #ifdef NDEBUG
  else
  {
    fprintf(stdout, "%03d> %s\n", fd, *line);
  }
  #endif

  return lineIndex;
}
/* End recvline */

// recvlinef(int fd, char *format, ...)
// Description: Receive data from a file descriptor (socket in this case).
//              Here you can specify a format string and points to variables 
//              to store the results in (like scanf)
// Input: int of file descriptor, char* the format string of the line, and any
//        pointers to variables to store results in
// Output: int number of bytes read
int recvlinef(int fd, char *format, ...)
{
  va_list argv;
  va_start(argv, format);
  
  int retVal = -1;
  char *line;
  int lineSize = recvline(fd, &line);
  
  if (lineSize > 0)
  {
    retVal = vsscanf(line, format, argv);
    free(line);
  }
  
  va_end(argv);
  
  return retVal;
}
/* End recvlinef */

int reverse_byte_orderi(int i)
{
  unsigned char c1, c2, c3, c4;
  c1 = i & 0xFF;
  c2 = (i >> 8) & 0xFF;
  c3 = (i >> 16) & 0xFF;
  c4 = (i >> 24) & 0xFF;
  return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

// Take an int and make it big-endian to send to a peer
int make_big_endian(int i)
{
  if(is_bigendian())
    return i;
  else
    return reverse_byte_orderi(i);
}

// Take an int and make it host byte order.  Useful for receiving data
// from peers
int make_host_orderi(int i)
{
  if(is_bigendian())
    return i;
  else
    return reverse_byte_orderi(i);
  
}

// print_metadata
//
// Summary : Prints human-readable output for metadata.
// Args    : torrentmedatdata_t* ptr
int print_metadata(torrentmetadata_t* m) {
  if( m ) {
    printf("Torrent metainfo:\n");
    printf("->info_hash  = ");
    int i = 0;
    for(i = 0; i < 5; i++) {
      printf("%08X ", m->info_hash[i]);
    } printf("\n");
    printf("->announce   = '%s'\n", m->announce);
    printf("->length     = %d\n", m->length);
    printf("->name       = '%s'\n", m->name);
    printf("->piece_len  = %d\n", m->piece_len);
    printf("->num_pieces = %d\n", m->num_pieces);
    printf("->pieces     = '%s'\n", m->pieces);
    return 0;
  } else {
    printf("print_metadata: no metadata\n");
    return -1;
  }

  return -2;
}

//get new peers from the tracker response
remote_peer_t* get_new_peers(tracker_data *tracker_response) {
    int i, j, foundMatch;
    remote_peer_t *cur;
    remote_peer_t *new = NULL;
    remote_peer_t *last_new = NULL;
    
    for (i = 0; i < tracker_response->numpeers; i++) {
        cur = g_current_connections;
        foundMatch = 0;
        for (j = 0; j < g_current_connections_number && cur != NULL; j++) {
            puts("Try to find match!!");
            if (strcmp(tracker_response->peers[i].ip, cur->ip) == 0 && tracker_response->peers[i].port == cur->port) {
                foundMatch = 1;
                puts("Found Match!!");
                break;
            }
            cur = cur->next;
        }
        if (!foundMatch) {
            remote_peer_t *new_peer = (remote_peer_t *)malloc(sizeof(remote_peer_t));
            new_peer->ip = (char *)malloc(strlen(tracker_response->peers[i].ip));
            strcpy(new_peer->ip, tracker_response->peers[i].ip);
            new_peer->port = tracker_response->peers[i].port;
            new_peer->next = NULL;
            
            if (new == NULL) {
                new = new_peer;
                last_new = new_peer;
            }
            else {
                last_new->next = new_peer;
                last_new = new_peer;
            }
        }
    }
    return new;
}

//add peer to list, return 1 if success, return 0 if fails
int add_to_list(remote_peer_t **list, remote_peer_t *new_peer) {
    if (new_peer != NULL) {
        if (*list != NULL) {
            remote_peer_t *cur = *list;
            while (cur->next != NULL) {
                cur = cur->next;
            }
            cur->next = (remote_peer_t *)malloc(sizeof(remote_peer_t));
            cur->next->ip = (char *)malloc(strlen(new_peer->ip));
            strcpy(cur->next->ip, new_peer->ip);
            cur->next->port = new_peer->port;
            cur->next->next = NULL;
            return 1;
        }
        else {
            *list = (remote_peer_t *)malloc(sizeof(remote_peer_t));
            (*list)->ip = (char *)malloc(strlen(new_peer->ip));
            strcpy((*list)->ip, new_peer->ip);
            (*list)->port = new_peer->port;
            (*list)->next = NULL;
            return 1;
        }
    }
    return 0;
}

//remove peer from list, return 1 if success, return 0 if fails
int remove_from_list(remote_peer_t **list, remote_peer_t *peer) {
    remote_peer_t *cur = *list;
    remote_peer_t *before = *list;
    while ((cur != NULL) && (strcmp(cur->ip, peer->ip) != 0 || cur->port != peer->port)) {
        before = cur;
        cur = cur->next;
    }
    if (cur == NULL) {
        return 0;
    }
    else if (cur == *list){
        *list = (*list)->next;
        free(cur->ip);
        free(cur);
        return 1;
    }
    else {
        before->next = cur->next;
        free(cur->ip);
        free(cur);
        return 1;
    }
}

void free_list(remote_peer_t *list) {
    remote_peer_t *cur;
    while (list != NULL) {
        cur = list;
        list = list->next;
        free(cur->ip);
        free(cur);
    }
}

//returns 1 if duplicate, 0 if not.
int dupID(remote_peer_t *current_list, remote_peer_t *new_peer) {
    int i;
    int foundDup;
    
    while (current_list != NULL) {
        foundDup = 1;
        if (strlen(current_list->id) != strlen(new_peer->id)) {
            current_list = current_list->next;
            foundDup = 0;
            continue;
        }
        
        for (i = 0; i < strlen(current_list->id); i++) {
            if (current_list->id[i] != new_peer->id[i]) {
                current_list = current_list->next;
                foundDup = 0;
            }
        }
        if (foundDup) {
            return 1;
        }
    }
    return 0;
}
