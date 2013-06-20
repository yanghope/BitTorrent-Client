#ifndef _PWP_CONNECTION_MANAGER_H
#define _PWP_CONNECTION_MANAGER_H

#include "./../util/jhl_lib.h"
#include "./../util/btdata.h"
#include "pwp_bitfield.h"
#include "pwp_message_header.h"
#include "pwp_status_msgs.h"
#include "pwp_data_msgs.h"
#include "./../util/queue.h"

#define RAND_BOOL (rand() % 2 == 1)
// Keep-alive message.
pwp_msg_t g_pwp_keep_alive_msg;// = {0, 0, NULL};

// Function definitions.
void fail(int retval, int pwp_index);
void* pwp_connection_manager(void * arg_); // pwp_connection_manager_argument_t*
// int pwp_msg_send_simple  (int socket_fd,                     char message_id, char* payload);
// int pwp_msg_send_variable(int socket_fd, int message_length, char message_id, char* payload);

int pwp_msg_auto_send(pwp_state_t* state);
int pwp_msg_process  (pwp_state_t* state);
int pwp_msg_recv     (pwp_state_t* state);

void* pwp_connection_listener(void* arg);
int pwp_msg_send(int socket_fd, int length, char id, char* payload);
#endif
