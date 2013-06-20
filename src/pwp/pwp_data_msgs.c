#include "pwp_data_msgs.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "./../util/sha1.h"
#include "./../util/util.h"

/*****
 request message functions
 *****/

/*
 Puts pwp_request_t into a malloc-ed char array/sendable byte array.
 Returns -1 on failure.
 */
int make_request_payload(pwp_request_t req, char *payload) {
	jhl_clean(payload, 12);
    
    printf("making request from %d(%x) %d(%x) %d(%x).\n", req.index, req.index, req.offset, req.offset,req.length, req.length);
    int val = -1;
	little_to_big_endian(&req.index, val);
	memcpy(payload, &val, 4);
    
	little_to_big_endian(&req.offset, val);
	memcpy(payload+4, &val, 4);
    
	little_to_big_endian(&req.length, val);
	memcpy(payload+8, &val, 4);
    
	// printf("payload: %x\n", payload);
	// JHL. Commented out.
	return 0;
}

int get_request_piece_index(char *payload) {
	int val;
	memcpy(&val, payload, 4);
    //  return val;
	int ret;
	big_to_little_endian(val, &ret);
	return ret;
}

int get_request_offset(char *payload) {
	int val;
	memcpy(&val, &(payload[4]), 4);
    //  return val;
	int ret;
	big_to_little_endian(val, &ret);
	return ret;
}

int get_request_length(char *payload) {
	int val;
	memcpy(&val, &(payload[8]), 4);
    //  return val;
	int ret;
    big_to_little_endian(val, &ret);
	return ret;
}

/* takes the request msg's payload and the peer_request_queue.
 returns 0 on success, -1 on failure
 */
int process_request(pwp_state_t* state, char *payload) {
    pwp_request_t *req = malloc(sizeof(pwp_request_t));
    //printf("payload: %x size: %d\n", payload, sizeof(payload));
    
    if ( (req->index = get_request_piece_index(payload)) < 0) return -1;
	if ( (req->offset = get_request_offset(payload)) < 0) return -1;
	if ( (req->length = get_request_length(payload)) < 0) return -1;
    
    queue_add(state->queue_requests_from_peer, req);
	return 0;
}

int determine_request(pwp_state_t* state) {
    printf("&&&&&&&&&&&in determine_request DIFF BITFIELD: &&&&&&&&&&&&&&&");
    bitf_print(state->diff_bitfield, g_bitfield_size);
    
    //if we are not dling a piece yet
    // Request a new piece.
    if (state->current_piece_index == -1) {
        state->current_piece_index = bitf_obtainable_random(state->diff_bitfield);
        if( state->current_piece_index == -1 ) {
            // In principle the program should not enter this if-statement.
            printf("data_msgs$determine_request: error: bitf_obtainable() returns -1\n");
            return -1;
            // If this does indeed take place, however, perhaps we should do something in the
            // Auto-send loop upon recognizing the failure.
        }
        // JHL.
        // Check if this piece is the last piece (possibly smaller than fixed piece length).
        if (state->current_piece_index == g_filelen / g_torrentmeta->piece_len) {
            state->piece_len = g_filelen % g_torrentmeta->piece_len;
            printf("\n~~~~~~~~~~~~~~~~We are requesting the tail piece. The piece length is: %d as opposed to %d\n", state->piece_len, g_torrentmeta->piece_len);
            
        } else {
            // JHL.
            // This will set state->piece len to a valid value.
            state->piece_len = g_torrentmeta->piece_len;
        }
        
        printf("\n\nindex chosen: %d\n\n", state->current_piece_index);
        state->block_index = 0;
        jhl_clean(state->piece_in_progress, g_torrentmeta->piece_len);
    }
    
    //make next request (w/ index + offset vars as in state
    state->request_to_peer.index = state->current_piece_index;
    state->request_to_peer.offset = state->block_index * BLOCK_SIZE;
    
    if (state->block_index == state->piece_len / BLOCK_SIZE) {
        state->request_to_peer.length = state->piece_len % BLOCK_SIZE;
        printf("block is not full size. requested size: %d\n", state->request_to_peer.length);
    }
    else {
        state->request_to_peer.length = BLOCK_SIZE;
        printf("block is full size\n");
    }
    return 0;
}


/*****
 piece message functions
 *****/

int make_piece_payload(pwp_request_t req, char *payload) {
    //file is in char* g_filedata
    //piece_size is int g_torrentmeta->piece_len
    //  printf("making piece. req index: %d, offset: %d, length: %d\n");
    //fill in index (from req)
    //fill in begin (offset) (from req)
    jhl_clean(payload, 12 * sizeof(char));
    
    int val = -1;
    little_to_big_endian(&req.index, val);
    // printf("val: %x\n", val);
    // memcpy(payload, &req.index, 4);
    memcpy(payload, &val, 4);
    
    little_to_big_endian(&req.offset, val);
    // printf("val: %x\n", val);
    // memcpy(payload+4, &req.offset, 4);
    memcpy(payload+4, &val, 4);
    
    //find place in file i want to copy
    int index = g_torrentmeta->piece_len * req.index + req.offset;
    //copy file for the length bits until file ends or length is satisfied
    
    //  printf("memcpying\n");
    memcpy(payload+8, g_filedata+index, req.length);
    //  printf("endian converting\n");
    //  endian_convert(payload+8, req.length);
    return 0;
}

int process_piece(pwp_state_t* state, char *payload) {
    //assumes that we still want the piece.
    pwp_request_t req;
    req.index = get_request_piece_index(payload);
    req.offset = get_request_offset(payload);
    req.length = state->request_to_peer.length;
    
    printf("state: %d/%d, request=%d/%d\n", state->request_to_peer.index,
           state->request_to_peer.offset, req.index, req.offset);
    if (state->request_to_peer.index != req.index || state->request_to_peer.offset != req.offset) {
        //under current implementation, this shouldn't happen.
        printf("state's index/offset and request's index/offset don't match!--this shouldn't happen!\n");
        return -1;
    }
    
    memcpy(state->piece_in_progress+ req.offset, payload+8, state->request_to_peer.length);
    
    //compute SHA1 hash of the piece index we just filled
    SHA1Context sha;
    SHA1Reset(&sha);
    unsigned char Message_Digest[20];
    SHA1Input(&sha, (const unsigned char*)state->piece_in_progress, state->piece_len);	//hopefully if the piece is smaller than this (the end piece) this function will still do it's job.
    //SHA1Input(&sha, (const unsigned char*)state->piece_in_progress, g_torrentmeta->piece_len);	//hopefully if the piece is smaller than this (the end piece) this function will still do it's job.
    if(!SHA1Result(&sha, Message_Digest))
    {
        printf("FAILURE in SHA1Result\n");
    }
    //printf("comparing sha1s of dled piece and the torrent's given sha1\n");
    //if we are finished dling the piece from the peer (hash is the same)
    if (strncmp((g_torrentmeta->pieces) + (req.index * 20), (char *)Message_Digest, 20) == 0) {
        //put dled piece in the file
        //printf("WE GOT A PERFECT PIECE!!!\n\n\n");
        int location = req.index * g_torrentmeta->piece_len;
        memcpy(g_filedata+location, state->piece_in_progress, state->piece_len);
        
        //set bitfields appropriately
        bitf_set_bit(g_bitfield, req.index);
        bitf_clear_bit(state->diff_bitfield, req.index);
        
        pthread_mutex_lock(&g_tracker_report_variables_mutex);
        g_downloaded += state->piece_len;
        g_left -= state->piece_len;
        pthread_mutex_unlock(&g_tracker_report_variables_mutex);
        
        //printf out progress
        //      printf("**************PROGRESS: %f**************\n", bitf_calculate_percentage(g_bitfield));
        
        //reset state's piece variables
        reset_piece(state);
        
        return req.index;
    }
    else {
        state->block_index++;
        return -1;
    }
}

/*****
 cancel message functions
 *****/

int reset_piece(pwp_state_t* state) {
	state->current_piece_index = -1;
	state->block_index = -1;
	jhl_clean(state->piece_in_progress, g_torrentmeta->piece_len);
	return 0;
}

int make_cancel_payload(pwp_request_t req, char *payload) {
	return make_request_payload(req, payload);
}

int pwp_request_t_compare (void *obja, void *objb) {
	pwp_request_t *a = (pwp_request_t *) obja;
	pwp_request_t *b = (pwp_request_t *) objb;
	return (a->index == b->index && a->offset == b->offset && a->length == b->length) ? 1 : 0;
}

int process_cancel(char *payload, pwp_state_t* state) {
	//update bitfield <-??? what??? i think i was sleepy...
	pwp_request_t req;
	if ( (req.index = get_request_piece_index(payload)) < 0) return -1;
	if ( (req.offset = get_request_offset(payload)) < 0) return -1;
	if ( (req.length = get_request_length(payload)) < 0) return -1;
    //should the length be taken into account for this...?
    //depends on the implementation of the peer's making of the requests - will the length at a certain index/offset always be the same?
	//remove the request from peer's request queue if the request exists
	queue_remove_all_obj(state->queue_requests_from_peer, &req, pwp_request_t_compare);
	return 0;
}
