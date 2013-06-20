#ifndef _PWP_STATUS_MSGS_H
#define _PWP_STATUS_MSGS_H

#include "./../util/btdata.h"
#include "./../util/jhl_lib.h"
#include "pwp_message_header.h"
#include "pwp_bitfield.h"

int handshake_init();
int handshake_send(int socket_fd);
int handshake_recv(int socket_fd, char* peer_id);
int keep_alive_msg_send(int socket_fd);
int payloadless_msg_send(int socket_fd, pwp_msg_t* message);

#endif
