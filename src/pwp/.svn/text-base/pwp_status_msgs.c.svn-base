// CS 60 -- Project -- Team2
//
// Defines functions involving status messages, including handshake.

#include "pwp_status_msgs.h"
#include "../util/util.h"

extern pwp_msg_t g_pwp_keep_alive_msg;

// Refer to pwp_message_header.h$pwp_handshake_msg_t for the contents of this message.xs
int handshake_init() {
  pthread_mutex_lock(&g_pwp_handshake_init_lock);
  if( g_pwp_handshake_init == 0 ) {
    jhl_clean(&g_pwp_handshake_msg, 0);
    g_pwp_handshake_msg.name_length = 19;
    memcpy(g_pwp_handshake_msg.protocol_name, BT_PROTOCOL, 19 * sizeof(char));
    // Leave reserved as zeroes.
    memcpy(g_pwp_handshake_msg.info_hash, g_infohash, 5 * sizeof(int));
    memcpy(g_pwp_handshake_msg.peer_id, g_my_id, 20 * sizeof(char));

    pthread_mutex_unlock(&g_pwp_handshake_init_lock);
    if( JHL_LOG ) {
      printf("g_pwp_handshake_msg initialized:\n");
      printf("{%hu, %s, (reserved), (info hash), %s}(%zu)\n",
       g_pwp_handshake_msg.name_length,
       g_pwp_handshake_msg.protocol_name,
       g_pwp_handshake_msg.peer_id,
       sizeof(g_pwp_handshake_msg));
    }
    return 1;
  }
  pthread_mutex_unlock(&g_pwp_handshake_init_lock);
  return 0;
}

int handshake_send(int socket_fd) {
  int retval;

  retval = jhl_socket_send(socket_fd, (char*) &g_pwp_handshake_msg, sizeof(pwp_handshake_msg_t));
  if( retval < 0 ) {
    jhl_log("pwp$handshake_send$send");
  }

  return retval;
}

// If I am the connecting peer, I know information about peer.
// If I am the accepting peer, I don't. peer_id must be a null string.
int handshake_recv(int socket_fd, char* peer_id) {
  char buffer[20];
  jhl_clean(buffer, 0);
  int retval = 0;

  // Receive 1 byte for name length.
  unsigned char name_length = 0;
  retval = jhl_socket_receive(socket_fd, (char*) &name_length, sizeof(unsigned char));
  if( retval <= 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket=%d: name_length(%d) fails\n", socket_fd, name_length);
    return -1;
  }
  // If not the same protocol name length (i.e., different protocol), drop.
  if( name_length != 19 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket=%d: name_length != 19\n", socket_fd);
    return -101;
  }

  // Receive 19 bytes for protocol name.
  retval = jhl_socket_receive(socket_fd, buffer, 19 * sizeof(char));
  if( retval <= 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket=%d: protocol_name fails\n", socket_fd);
    return -2;
  }
  // If not the same protocol, drop.
  if( strncmp(buffer, BT_PROTOCOL, 19) != 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket=%d: protocol_name != \"BitTorrent protocol\"\n", socket_fd);
    jhl_log(buffer);
    return -102;
  }

  // Receive 8 reserved bytes.
  retval = jhl_socket_receive(socket_fd, buffer, 8 * sizeof(char));
  if( retval <= 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket %d: reserved fails\n", socket_fd);
    return -3;
  }

  // Receive 20 bytes for SHA-1 info hash.
  jhl_clean(buffer, 0);
  retval = jhl_socket_receive(socket_fd, buffer, 5 * sizeof(int));
  if( retval <= 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket: %d: info_hash fails\n", socket_fd);
    return -4;
  }
  // If we're after different files, drop.
  if( strncmp(buffer, (char*) g_infohash, 20) != 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket %d: info_hash != (my info hash)\n", socket_fd);
    return -104;
  }

  // Receive 20 bytes for peer ID.
  jhl_clean(buffer, 0);
  retval = jhl_socket_receive(socket_fd, buffer, 20 * sizeof(char));
  if( retval <= 0 ) {
    if( JHL_LOG ) printf("pwp$handshake_recv$socket %d: peer_id fails\n", socket_fd);
    return -5;
  }
  printf("received ID='%s'\n", buffer);
  // If we have the same ID, drop.
  if( strncmp(buffer, g_my_id, 20) == 0 ) {
    if( JHL_LOG )  printf("pwp$handshake_recv$socket %d: peer_id == (my ID)(%s)\n", socket_fd, buffer);
    return -105;
  }
  // Copy the ID into peer_id, so that we can check later whether duplicate or not.
  else {
    memcpy(peer_id, buffer, 20 * sizeof(char));
    peer_id[20] = '\0';
  }

  return 0;
}

int keep_alive_msg_send(int socket_fd) {
  int retval;

  // Send size (0).
  //int size_in_big_endian = make_big_endian(0);
  int size_in_big_endian = 0;
  retval = jhl_socket_send(socket_fd, (char*) &size_in_big_endian, sizeof(int));
  if( retval < 0 ) {
    jhl_log("pwp$keep_alive_send$fails");
  }

  // Don't send any more (because size is 0).
  //  retval = jhl_socket_send(socket_fd, (char*) &g_pwp_keep_alive_msg, sizeof(pwp_msg_t));
  //  if( retval < 0 ) {
  //    jhl_log("pwp$keep_alive_send$send fails");
  //  }

  if( JHL_LOG ) printf("  pwp$keep_alive_send:to socket(%d)\n", socket_fd);

  return retval;
}


int payloadless_msg_send(int socket_fd, pwp_msg_t* message) {
  int retval = 0;

//  printf("  message length: %d\n", message->message_length);
  //int big_endian_buffer = make_big_endian(message->message_length);
  int big_endian_buffer = message->message_length;
  endian_convert((char*) &big_endian_buffer, sizeof(int));
//  printf("  endianed length: %d\n", big_endian_buffer);
  retval = jhl_socket_send(socket_fd, (char*) &big_endian_buffer, sizeof(int));
  if( retval < 0 ) {
    jhl_log("pwp$pwp_msg_send$payloadless_msg_send$message_length fails");
    return -1;
  }

  retval = jhl_socket_send(socket_fd, &message->message_id, sizeof(char));
  if( retval < 0 ) {
    jhl_log("pwp$pwp_msg_send$payloadless_msg_send$message_id fails");
    return -2;
  }

  return 0;
}
