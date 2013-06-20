#ifndef _PHP_REQUEST_H
#define _PHP_REQUEST_H

#include "./../util/jhl_lib.h"
#include "./../util/queue.h"
#include "pwp_bitfield.h"
#include "./../util/btdata.h"
#include "pwp_message_header.h"

/*****
helper functions to ease message processing
*****/


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*****
	request message functions
*****/
int make_request_payload(pwp_request_t req, char *payload);

int get_request_piece_index(char *payload);

int get_request_offset(char *payload);

int get_request_length(char *payload);

int process_request(pwp_state_t* state, char *payload);

/* called to create a pwp_request_t in pwp_msg_auto_send */
int determine_request(pwp_state_t* state);


/*****
	piece message functions
*****/
int make_piece_payload(pwp_request_t req, char *payload);

int process_piece(pwp_state_t* state, char *payload);


/*****
	cancel message functions
*****/
/* method called in autosend when we want to cancel a piece
(when the index of the piece we're donwloading is already finished
in the global bitfield*/
int reset_piece(pwp_state_t* state);

int make_cancel_payload(pwp_request_t req, char *payload);

int pwp_request_t_compare (void *obja, void *objb);
int process_cancel(char *payload, pwp_state_t* state);
#endif
