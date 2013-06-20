// pwp_connection_manager.c
//
// CS 60 -- Project -- Team2
//
// Defines pwp_connection_manager(). This method will handle a connection with an individual peer
#include "pwp_connection_manager.h"
#include "./../util/util.h"

extern int g_pwp_handshake_init;

// +------+
// |DESIGN|
// +------+
//
// INITIALIZE
// 0.1. Check if connection is worth making (drop duplicate IDs).
// 0.2. Initialize variables.
//
// HANDSHAKE
// (if connecting)
// 1.1a. Send my handshake message.
// 1.2a. Receive your handshake message.
// 1.3a. Process your handshake message (if invalid or duplicate, quit).
// (if accepting)
// 1.1b. Receive your handshake message.
// 1.2b. Process your handshake message (if invalid or duplicate, quit).
// 1.3b. Send my handshake message.
//
// BITFIELD
// (if connecting)
// 2a. Send bitfield.
// (if accepting)
// 2.1b. Receive a message.
// 2.2b. Process the message.
// 2.3c. Send bitfield.
//
// GENERAL PWP COMMUNICATION LOOP
// while( connection on ) {
//   Receive a message (if no message for a timeout-amount of time, drop connection).
//   Process the message.
//   Auto-send a message.
// }
void pwp_state_fin(pwp_state_t* state);
void* pwp_connection_manager(void* arg_) {
    jhl_log("((((((((((((((((((((pwp_connection_manager STARTED!!!)))))))))))))))");
    
    pwp_arg_t arg;
    memcpy((char*) &arg, (char*) arg_, sizeof(pwp_arg_t));
    free(arg_);
    
    int my_pwp_index = -1;
    int retval = 0;
    
    // Check for the maximum connection limit.
    int pwp_full = 1;
    //    printf("waiting for lock in pwp_connection manager for %s:%d\n", arg.ip, arg.port);
    pthread_mutex_lock(&g_pwp_connections_mutex);
    //    printf("has lock in pwp_connection_manager for %s:%d\n", arg.ip, arg.port);
    
    
    for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
        if( g_pwp_connections[i].socket_fd < 0 ) {
            pwp_full = 0;
            my_pwp_index = i;
            //      g_pwp_connections[i].socket_fd = arg.socket_fd;
            break;
        }
    }
    if( pwp_full == 1 ) {
        jhl_log("pwp: no more room for PWP connections: exit");
        pthread_mutex_unlock(&g_pwp_connections_mutex);
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&g_pwp_connections_mutex);
    //    printf("$$$First unlock 1 in pwp_connection_manager for %s:%d\n", arg.ip, arg.port);
    // Check for duplicate connection.
    //    for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
    //        if( g_pwp_connections[i].socket_fd >= 0 &&
    //           strncmp(g_pwp_connections[i].ip, arg.ip, 16) == 0 &&
    //           g_pwp_connections[i].port == arg.port) {
    //            jhl_log("pwp: duplicate connection: exit");
    //            pthread_mutex_unlock(&g_pwp_connections_mutex);
    //            pthread_exit(NULL);
    //        }
    //    }
    // Not duplicate: register the information in g_pwp_connections.
    //    memcpy(g_pwp_connections[my_pwp_index].ip, arg.ip, 16);
    //    g_pwp_connections[my_pwp_index].port = arg.port;
    //    g_pwp_connections[my_pwp_index].connecting = arg.connecting;
    
    if (arg.connecting) {
        //        printf("##################trying to connect to %s:%d!!!!\n", arg.ip, arg.port);
        retval = jhl_socket_connect(arg.ip, arg.port);
        if (retval < 0) {
            printf("%s:%d's", arg.ip, arg.port);
            jhl_log("pwp: unsuccessful socket connection!!!!!!!!!!!!unclock and thread exits!!!!!");
            //            pthread_mutex_unlock(&g_pwp_connections_mutex);
            pthread_exit(NULL);
        }
        else {
            pthread_mutex_lock(&g_pwp_connections_mutex);
            memcpy(g_pwp_connections[my_pwp_index].ip, arg.ip, 16);
            g_pwp_connections[my_pwp_index].port = arg.port;
            g_pwp_connections[my_pwp_index].connecting = arg.connecting;
            g_pwp_connections[my_pwp_index].socket_fd = retval;
            pthread_mutex_unlock(&g_pwp_connections_mutex);
            arg.socket_fd = retval;
        }
        //        printf("#################### CONNECTED to %s:%d\n", arg.ip, arg.port);
    }
    else {
        pthread_mutex_lock(&g_pwp_connections_mutex);
        memcpy(g_pwp_connections[my_pwp_index].ip, arg.ip, 16);
        g_pwp_connections[my_pwp_index].port = arg.port;
        g_pwp_connections[my_pwp_index].connecting = arg.connecting;
        g_pwp_connections[my_pwp_index].socket_fd = arg.socket_fd;
        pthread_mutex_unlock(&g_pwp_connections_mutex);
    }
    //    pthread_mutex_unlock(&g_pwp_connections_mutex);
    //    printf("$$$Second unlock in pwp_connection_manager for %s:%d\n", arg.ip, arg.port);
    
    // Establish socket connection if connecting.
    //  if( arg.connecting ) {
    //    retval = jhl_socket_connect(arg.ip, arg.port);
    //    if( retval < 0 ) {
    //      pthread_mutex_lock(&g_pwp_connections_mutex);
    //      jhl_clean(g_pwp_connections[my_pwp_index].ip, 16 * sizeof(char));
    //      g_pwp_connections[my_pwp_index].port = 0;
    //      pthread_mutex_unlock(&g_pwp_connections_mutex);
    //      jhl_log("pwp: unsuccessful socket connection");
    //      pthread_exit(NULL);
    //    } else {
    //      pthread_mutex_lock(&g_pwp_connections_mutex);
    //      for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
    //	if( strncmp(g_pwp_connections[i].ip, arg.ip, 16) == 0 &&
    //	    g_pwp_connections[i].port == arg.port ) {
    //	  g_pwp_connections[i].socket_fd = retval;
    //	  break;
    //	}
    //      }
    //      pthread_mutex_unlock(&g_pwp_connections_mutex);
    //    }
    //    arg.socket_fd = retval;
    //  }
    // After this point, information about my peer is registered in g_pwp_connections.
    // fail() will remove this information.
    
    
    // Connection manager will now start its main tasks.
    /*if( JHL_LOG ) {
     printf("----PWP CONNECTION----\n");
     printf("pwp: %s at %s:%d(socket %d, ", arg.id, arg.ip, arg.port, arg.socket_fd);
     if( arg.connecting ) {
     printf("(connecting)");
     } else {
     printf("(accepting)");
     }
     printf("\n");
     }
     */
    // Initialize pwp_state_t structure.
    pwp_state_t* state = (pwp_state_t*) jhl_malloc(sizeof(pwp_state_t));
    state->socket_fd = arg.socket_fd;
    state->pwp_index = my_pwp_index;
    state->am_choking = 1;
    state->was_choking = 1;
    state->am_choked = 1;
    state->was_choked = 1;
    state->am_interested = 0;
    state->was_interested = 0;
    state->am_interesting = 0;
    state->was_interesting = 0;
    state->peer_last_heard = 0L;
    state->me_last_talked = 0L;
    state->my_bitfield = bitf_create(g_bitfield_size);
    state->peer_bitfield = bitf_create(g_bitfield_size);
    state->diff_bitfield = bitf_create(g_bitfield_size);
    state->recv_msg_id = -1;
    state->recv_msg_size = -1;
    state->recv_msg_payload = NULL;
    state->current_piece_index = -1;
    state->queue_requests_from_peer = queue_init();
    jhl_clean(&state->request_to_peer, 0);
    state->block_index = -1;
    state->piece_in_progress = (char*) jhl_malloc(g_torrentmeta->piece_len * sizeof(char));
    
    
    // Handshake.
    if( arg.connecting ) {
        handshake_init();
        retval = handshake_send(state->socket_fd);
        if( retval < 0 ) {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s:%d's pwp$CONNECTING handshake: FAIL!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
        retval = handshake_recv(state->socket_fd, arg.id);
        if( retval < 0 )  {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s:%d's pwp$CONNECTING handshake: FAIL!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
    } else {
        retval = handshake_recv(state->socket_fd, arg.id);
        if( retval < 0 ) {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s:%d's pwp$CONNECTED handshake: FAIL!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
        handshake_init();
        retval = handshake_send(state->socket_fd);
        if( retval < 0 ) {
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s:%d's pwp$CONNECTED handshake: FAIL!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
    }
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~%s:%d's pwp$handshake: DONE!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", arg.ip, arg.port);
    //  jhl_log("~~~~~~~~~~~~~~~~~~~~~~~~~~~~pwp$handshake: DONE!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    
    // Handshake done.
    // If the peer is duplicate, quit now. Otherwise update my PWP information.
    pthread_mutex_lock(&g_pwp_connections_mutex);
    for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
        if( strncmp(arg.id, g_pwp_connections[i].id, 21) == 0 ) {
            printf("################%s:%d's", arg.ip, arg.port);
            jhl_log("pwp: duplicate ID: exit##############");
            //XNH: need to close the connection
            close(arg.socket_fd);
            jhl_clean(&g_pwp_connections[my_pwp_index], sizeof(pwp_arg_t));
            g_pwp_connections[my_pwp_index].socket_fd = -1;
            pthread_mutex_unlock(&g_pwp_connections_mutex);
            pthread_exit(NULL);
        }
    }
    memcpy(g_pwp_connections[my_pwp_index].id, arg.id, 21);
    pthread_mutex_unlock(&g_pwp_connections_mutex);
    
//    puts("CHECK DUPLICATE DONE!!!");
    
    // Update communication time.
    state->me_last_talked = jhl_timer_get_time();
    state->peer_last_heard = jhl_timer_get_time();
    
    // Bitfield.
    // Prepare bitfield.
    //    puts("%%%%%%%%%%%%%%%%waiting for bitfield mutex!!!!!!!!!!!!!!!!");
    pthread_mutex_lock(&g_bitfield_mutex);
    memcpy((char*) state->my_bitfield, (char*) g_bitfield, g_bitfield_size * sizeof(char));
    pthread_mutex_unlock(&g_bitfield_mutex);
    // peer_bitfield is initialized as empty. If a peer doesn't send its bitfield,
    // then the empty bitfield is the correct representation.
    if( JHL_LOG ) { printf("my own "); bitf_print(g_bitfield, g_bitfield_size); }
    
    // Send bitfield. If accepting, receive a message first.
    if( !arg.connecting ) {
        retval = pwp_msg_recv(state); if( retval < 0 ) fail(retval, state->pwp_index);
        retval = pwp_msg_process(state);
    }
    pthread_mutex_lock(&g_bitfield_mutex);
    char* bitfield_buffer_big_endian = (char*) jhl_malloc(g_bitfield_size * sizeof(char));
    memcpy(bitfield_buffer_big_endian, g_bitfield, g_bitfield_size * sizeof(char));
    
    //DON'T CONVERT!!!!
//    endian_convert(bitfield_buffer_big_endian, g_bitfield_size);
    pwp_msg_send(state->socket_fd, g_bitfield_size+1, BITFIELD, bitfield_buffer_big_endian);
    free(bitfield_buffer_big_endian);
    pthread_mutex_unlock(&g_bitfield_mutex);
//    puts("SEND BITFIELD DONE!!!");
    
    if( retval < 0 ) fail(retval, state->pwp_index);
    
    // Unchoke the peer.
    state->am_choking = 0;
    
    // At this point, it is always peer's turn to send a message: I can receive.
    while(1) {
//        jhl_log("pwp$main_loop: trying to receive...");
        //      printf("from %s:%d's\n", arg.ip, arg.port);
        retval = pwp_msg_recv(state);
        if( retval < 0 ) {
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^%s:%d's pwp$ RECEIVE msg: FAIL!!!!!!^^^^^^^^^^^^^^^^^^^^^\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
        
        printf("received something from %s:%d, processing\n", arg.ip, arg.port);
        retval = pwp_msg_process(state);
        if( retval < 0 ) {
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^%s:%d's pwp$ PROCESS msg: FAIL!!!!!!^^^^^^^^^^^^^^^^^^^^^^\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
        
        printf("^^^^^^^^^^^^^^^send to %s:%d:\n", arg.ip, arg.port);
//        while( (retval = pwp_msg_auto_send(state) == NO_MESSAGE_SENT) ) {
//            if( retval < 0 ) {
//                printf("^^^^^^^^^^^^^^^^^^^^^^^^^^%s:%d's pwp$ SEND msg: FAIL!!!!!!^^^^^^^^^^^^^^^^^^^^^^\n", arg.ip, arg.port);
//                fail(retval, state->pwp_index);
//            }
//            sleep(2);
//        }
        
        bitf_find_obtainable(state->diff_bitfield, state->peer_bitfield);
        //if the remote peer's data is not interesting any more
        if (bitf_obtainable_random(state->diff_bitfield) < 0) {
            //          state->was_interested = 1;
            state->am_interested = 0;
        }
        retval = pwp_msg_auto_send(state);
        if (retval < 0) {
            printf("^^^^^^^^^^^^^^^^^^^^^^^^^^%s:%d's pwp$ SEND msg: FAIL!!!!!!^^^^^^^^^^^^^^^^^^^^^^\n", arg.ip, arg.port);
            fail(retval, state->pwp_index);
        }
        else if (retval == NO_MESSAGE_SENT) {
//            sleep(3);
        }
        
        // Should I update a bitfield here for CANCEL messages?
    }
    
    // Free allocated memory.
    jhl_socket_close(state->socket_fd);
    pwp_state_fin(state);
    
    jhl_log("connection_manager$return");
    return 0;
}

void pwp_state_fin(pwp_state_t* state) {
    if( state->piece_in_progress ) {
        jhl_free(state->piece_in_progress);
        state->piece_in_progress = NULL;
    }
    jhl_free(state->my_bitfield);
    jhl_free(state->peer_bitfield);
    jhl_free(state->diff_bitfield);
    queue_free(state->queue_requests_from_peer);
    jhl_free(state);
    jhl_log("pwp_state_fin: successful");
}

void fail(int retval, int my_pwp_index) {
    if( JHL_LOG ) printf("pwp$fail: exiting (retval=%d)\n", retval);
    
    
    //XNH: need to close the connection;
    close(g_pwp_connections[my_pwp_index].socket_fd);
    //    puts("in fail!!!!wait for g_pwp_connections_mutex lock!!!!!!!!");
    pthread_mutex_lock(&g_pwp_connections_mutex);
    jhl_clean(&g_pwp_connections[my_pwp_index], sizeof(pwp_arg_t));
    g_pwp_connections[my_pwp_index].socket_fd = -1;
    pthread_mutex_unlock(&g_pwp_connections_mutex);
    pthread_exit(NULL);
}

int pwp_msg_auto_send(pwp_state_t* s) {
    // int maximum_number_of_messages_we_can_send_in_one_call_to_this_function = 1;
    // Unless the function decides that we have nothing to send, in which case we return.
    // while(maximum_number_of_messages_we_can_send_in_one_call_to_this_function--) {
    
    // At the end of while-block is a return.
    // In INTERESTED, however, it will call continue.
    while( 1 ) {
        // Used as a placeholder in sending HAVE messages.
//        int n = -1;
        s->me_last_talked = jhl_timer_get_time();
        
        // CHOKE
        // I decided to choke the peer.
        if( s->was_choking == 0 && s->am_choking == 1 ) {
            //if( peer_past->am_choking == 0 && peer_now->am_choking == 1 ) {
            jhl_log("pwp$auto_send: CHOKE");
            //pwp_msg_send_simple(s->socket_fd, CHOKE, NULL);
            if (pwp_msg_send(s->socket_fd, 1, CHOKE, NULL) < 0) {
                return -1;
            }
            // Update state history.
            s->was_choking = 1;
            //peer_past->am_choking = 1;
        }
        
        // UNCHOKE
        // I decided to unchoke the peer.
        else if( s->was_choking == 1 && s->am_choking == 0 ) {
            //else if( peer_past->am_choking == 1 && peer_now->am_choking == 0 ) {
            jhl_log("pwp$auto_send: UNCHOKE");
            //pwp_msg_send_simple(s->socket_fd, UNCHOKE, NULL);
            if (pwp_msg_send(s->socket_fd, 1, UNCHOKE, NULL) < 0) {
                return -1;
            }
            
            // Update state history.
            s->was_choking = 0;
            //peer_past->am_choking = 0;
        }
        
        // INTERESTED
        // I now take interest in the peer's pieces.
        else if( s->was_interested == 0 && s->am_interested == 1 ) {
            //else if( peer_past->am_interested == 0 && peer_now->am_interested == 1 ) {
            jhl_log("pwp$auto_send: INTERESTED");
            if (pwp_msg_send(s->socket_fd, 1, INTERESTED, NULL) < 0) {
                return -1;
            }
            
            // Update state history.
            s->was_interested = 1;
            continue;
        }
        
        // UNINTERESTED
        // I am now no longer interested in peer's pieces.
        else if( s->was_interested == 1 && s->am_interested == 0 ) {
            jhl_log("pwp$auto_send: UNINTERESTED");
            // pwp_msg_send_simple(s->socket_fd, UNINTERESTED, NULL);
            if (pwp_msg_send(s->socket_fd, 1, UNINTERESTED, NULL) < 0) {
                return -1;
            }
            // Update state history.
            s->was_interested = 0;
        }
        
        // HAVE
        // I now have this file piece.
//        else if( (n = bitf_find_unknown_to_peer(s->my_bitfield)) > 0 ) {
//            jhl_log("pwp$auto_send: HAVE$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
////            int val = 0;
////            little_to_big_endian(&n, val);
////            pwp_msg_send(s->socket_fd, 5, HAVE, (char*) &val);
//            
//            int val = htonl(n);
//            pwp_msg_send(s->socket_fd, 5, HAVE, (char *) &val);
//            // Update my bitfield, too, so that I don't keep sending HAVE messages.
//            bitf_set_bit(s->my_bitfield, n);
//            // Update diff bitfield, so that I don't keep sending REQUEST messages.
//            bitf_clear_bit(s->diff_bitfield, n);
//            continue;
//        }
        
        // BITFIELD
        // This is sent only as the first message.
        // I treat this a special case and address it directly in conection_manager().
        
        // REQUEST
        else if( s->am_choked == 0 && s->am_interested == 1 && (queue_is_empty(s->queue_requests_from_peer) == 1) &&
                bitf_obtainable_random(s->diff_bitfield) >= 0) {
            
            printf("pwp$auto_send: REQUEST to socket %d: \n", s->socket_fd);
            determine_request(s); //choose piece
            char *temp_payload = (char*) jhl_malloc(12 * sizeof(char));
            make_request_payload(s->request_to_peer, temp_payload);
            if (pwp_msg_send(s->socket_fd, 13, REQUEST, temp_payload) < 0) {
                free(temp_payload);
                return -1;
            }
            free(temp_payload);
        }
        
        // PIECE
        else if(s->am_interesting == 1 && queue_is_empty(s->queue_requests_from_peer) == 0) {
            jhl_log("pwp$auto_send: PIECE");
            //pop a request off the peer request queue
            pwp_request_t *req = NULL;
            while (req == NULL) {
              req = (pwp_request_t *)queue_remove(s->queue_requests_from_peer);
            }
           
            char *temp_payload = (char*) jhl_malloc((req->length + 8) * sizeof(char));
            make_piece_payload(*req, temp_payload);
            if (pwp_msg_send(s->socket_fd, 9 + req->length, PIECE, temp_payload) < 0) {
                free(temp_payload);
                return -1;
            }
            free(temp_payload);
            free(req);
        }
        
        // CANCEL
        else if( 0 ) {  //todo: if we're in endgame and need to cancel a request, make a condition for this
            jhl_log("pwp$auto_send: CANCEL");
            
            char *temp_payload = (char*) jhl_malloc(12 * sizeof(char));
            make_cancel_payload(s->request_to_peer, temp_payload);
            if (pwp_msg_send(s->socket_fd, 13, CANCEL, temp_payload) < 0) {
                free(temp_payload);
                return -1;
            }
            free(temp_payload);
        }
        
        // KEEP_ALIVE
        else if( jhl_timer_get_time() - s->me_last_talked > KEEP_ALIVE_INTERVAL ) {
            printf("pwp$auto_send to socket %02d: KEEP-ALIVE  : choked: %d, interested: %d, queue_is_empty: %d\n",
                   s->socket_fd,
                   s->am_choked, s->am_interested, (queue_is_empty(s->queue_requests_from_peer)));
            if (pwp_msg_send(s->socket_fd, 0, KEEP_ALIVE, NULL) < 0) {
                return -1;
            }
        }
        
        // Else wait a little.
        else {
            printf("pwp$auto_send to socket %02d: NOTHING SENT: choked: %d, interested: %d, queue_is_empty: %d\n",
                   s->socket_fd,
                   s->am_choked, s->am_interested, (queue_is_empty(s->queue_requests_from_peer)));
            return NO_MESSAGE_SENT;
        }
        
        // Update the last time of message sent.
//        s->me_last_talked = jhl_timer_get_time();
        
        return 0;
    }
//    return 0;
}

int pwp_msg_process(pwp_state_t* s) {
    
    // Record the time of arrival.
    s->peer_last_heard = jhl_timer_get_time();
    
    int piece_index;
    int val;
    
    if( JHL_LOG ) printf("pwp$process: message_id=%d\n", s->recv_msg_id);
    switch( s->recv_msg_id ) {
            
        case KEEP_ALIVE:
            puts("pwp$process: processing KEEP_ALIVE");
            // KEEP_ALIVE
            // Take no particular action except update the time last heard (done above).
            return 0;
            
        case CHOKE:
            puts("pwp$process: processing CHOKE");
            // CHOKE
            // The peer decides to choke me.
            s->am_choked = 1;
            return 0;
            
        case UNCHOKE:
            puts("pwp$process: processing UNCHOKE");
            // UNCHOKE
            // The peer decides to unchoke me.
            s->am_choked = 0;
            return 0;
            
        case INTERESTED:
            puts("pwp$process: processing INTERESTED");
            // INTERESTED
            // The peer is now interested in my file pieces.
            s->am_interesting = 1;
            return 0;
            
        case UNINTERESTED:
            puts("pwp$process: processing UNINTERESTED");
            // UNINTERESTED
            // The peer is no longer interested in my file pieces.
            s->am_interesting = 0;
            return 0;
            
        case HAVE:
            // HAVE
            // The peer has a file piece.
            // See if I should take interest in this piece.
            
            puts("pwp$process: processing HAVE");
            //          piece_index = *(int*) s->recv_msg_payload;
            //          big_to_little_endian(piece_index, &piece_index);
            //          printf("pwp$process: processing HAVE %d", piece_index);
            //          bitf_set_bit(s->peer_bitfield, piece_index);
            //
            //          bitf_find_obtainable(s->diff_bitfield, s->peer_bitfield);
            //          piece_index = bitf_obtainable_random(s->diff_bitfield);
            //          if ( piece_index >= 0 ) {
            //              s->am_interested = 1;
            //          } else {
            //              s->am_interested = 0;
            //          }
            //          return 0;
            
            //bitf_set_bit(s->peer_bitfield, *(int*)s->recv_msg_payload);
            val = *(s->recv_msg_payload);
            big_to_little_endian(*((int *)s->recv_msg_payload), &val);
            printf("val: %d\n", val);
            bitf_set_bit(s->peer_bitfield,val);
            bitf_find_obtainable(s->diff_bitfield, s->peer_bitfield);
            piece_index = bitf_obtainable_random(s->diff_bitfield);
             
            if ( piece_index >= 0 ) {
                s->am_interested = 1;
            } else {
                s->am_interested = 0;
            }
            return 0;
            
        case BITFIELD:
            // BITFIELD
            // The peer informs me of which file pieces it have.
            
            puts("pwp$process: processing BITFIELD");
            if( s->recv_msg_size - 1 != g_bitfield_size ) {
                // In principle, code should not enter this if-block.
                
                printf("pwp$msg_process$BITFIELD: error: different bitfield sizes: mine %d versus peer's %d\n",
                       g_bitfield_size, s->recv_msg_size - 1);
                fail(-101, s->pwp_index);
                return -101;
            }
            
            memcpy((char*) s->peer_bitfield, s->recv_msg_payload, g_bitfield_size * sizeof(char));
            if( JHL_LOG ) {
                printf("&&&&&&&&&&&&&&&&Peer's BITFIELD&&&&&&&&&&&&&&&&");
                bitf_print(s->peer_bitfield, g_bitfield_size);
            }
            bitf_find_obtainable(s->diff_bitfield, s->peer_bitfield);
            piece_index = bitf_obtainable_random(s->diff_bitfield);
            if( piece_index >= 0 ) {
                s->am_interested = 1;
            }
            return 0;
            
        case REQUEST:
            jhl_log("pwp$process: processing REQUEST");
            process_request(s, s->recv_msg_payload);
            return 0;
            
        case PIECE:
            jhl_log("pwp$process: processing PIECE");
            int piece_idx = process_piece(s, s->recv_msg_payload);
            if (piece_idx >= 0) {
                int i;
                for (i = 0; i < MAX_PWP_CONNECTIONS; i++) {
                    if (g_pwp_connections[i].socket_fd >= 0) {
                        jhl_log("pwp$auto_send: HAVE$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
                        
                        int val = htonl(piece_idx);
                        pwp_msg_send(s->socket_fd, 5, HAVE, (char *) &val);
                    }
                }
            }
            return 0;
            
        case CANCEL:
            jhl_log("pwp$process: processing CANCEL");
            process_cancel(s->recv_msg_payload, s);
            
        default:
            // Code shouldn't reach here.
            jhl_log("pwp$msg_process: error: unidentified message type");
            //todo: drop the peer.
            return -1;
    }
    return -2;
}

int pwp_msg_recv(pwp_state_t* s) {
    // Clear the buffers.
    s->recv_msg_id = -1;
    s->recv_msg_size = -1;
    if( s->recv_msg_payload ) {
        jhl_free(s->recv_msg_payload);
        s->recv_msg_payload = NULL;
    }
    long  retval = 0;
    
    // Receive 4 bytes for size.
    // int big_endian_buffer_size = 0;
    //retval = jhl_socket_receive(s->socket_fd, (char*) &big_endian_buffer_size, sizeof(int));
//    retval = jhl_socket_receive(s->socket_fd, (char*) &s->recv_msg_size, sizeof(int));
    
//    puts("waiting for recv!!!");
    retval = recv(s->socket_fd, (char*) &s->recv_msg_size, sizeof(int), 0);
    if( retval <= 0 ) {
        puts("!!!!!!!!!!!!Connection DROPPED!!!!!!!!!");
        return -1;
    }
    
    //  endian_convert((char*) &s->recv_msg_size, sizeof(int));
    s->recv_msg_size = ntohl(s->recv_msg_size);
//    printf("################recv_msg_size %d#########\n", s->recv_msg_size);
    
    if( s->recv_msg_size < 0 ) {
        if( JHL_LOG ) printf("pwp$pwp_msg_recv$size: invalid value(%d)\n", s->recv_msg_size);
        //    endian_convert((char*) &s->recv_msg_size, sizeof(int));
        printf("rl: unconverted size: %d\n", htonl(s->recv_msg_size));
        return -101;
    }
    if( s->recv_msg_size == 0 ) {
        // The received message is a keep-alive message.
        // Nothign more is sent.
        //    jhl_socket_receive(s->socket_fd, &s->recv_msg_id, sizeof(char));
        s->recv_msg_id = KEEP_ALIVE;
        return 0;
    }
    
    // Receive 1 byte for the message_id.
    retval = jhl_socket_receive(s->socket_fd, &s->recv_msg_id, sizeof(char));
    if( retval < 0 ) {
        printf("pwp$pwp_msg_recv$message_id(%d) fails, size was %d", s->recv_msg_id, s->recv_msg_size);
        return -2;
    }
    if( s->recv_msg_id < 0 || 8 < s->recv_msg_id ) {
        printf("pwp$pwp_msg_recv$message_id(%d) is invalid, size was %d", s->recv_msg_id, s->recv_msg_size);
        return -102;
    }
    
    // Message has no payload.
    if( s->recv_msg_size == 1 ) {
        return 0;
    }
    
    // Message has payload.
    if (s->recv_msg_payload) 
      free(s->recv_msg_payload);
    s->recv_msg_payload = (char*) jhl_malloc((s->recv_msg_size - 1));
    //printf("about to receive payload\n");
    retval = 0;
    long m = 0;
    while (retval < s->recv_msg_size-1) {
        //printf("receiving\n");
        m = recv(s->socket_fd, s->recv_msg_payload+retval, (s->recv_msg_size-1) - retval, 0);
        if (m <= 0) {
            printf("^^^^^^^^^^^^^^^^^^^receive from socket FAILS!!!^^^^^^^^^^^^^^^^^^\n");
            return -200;
        }
        retval += m;
        
    }
    
    return 0;
}


int pwp_msg_send(int socket_fd, int length, char id, char* payload) {
    int endian = length;
    //send length
    endian_convert((char*) &endian, sizeof(int));
//    jhl_socket_send(socket_fd, (char*) &endian, sizeof(int));
    if (send(socket_fd, (char*) &endian, sizeof(int), 0) < 0) {
        perror("send in pwp_msg_send fails!!!");
        return -1;
    }
    
    //if length == 0, return;
    if (length == 0)
        return 0;
    
    //send msg id
//    jhl_socket_send(socket_fd, &id, sizeof(char));
    if (send(socket_fd, &id, sizeof(char), 0) < 0) {
        perror("send in pwp_msg_send fails!!!");
        return -1;
    }
    
    //if length == 1, return;
    if (length == 1)
        return 0;
    
    //(else)
    //send payload. needs to be endianned inside.
//    int retval = jhl_socket_send(socket_fd, payload, length - 1);
    if (send(socket_fd, payload, length - 1, 0) < 0) {
        perror("send in pwp_msg_send fails!!!");
        return -1;
    }
//    return retval;
    return 0;
}
