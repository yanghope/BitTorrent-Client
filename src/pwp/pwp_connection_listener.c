#ifndef _PWP_CONNECTION_LISTENER_C
#define _PWP_CONNECTION_LISTENER_C

#include "pwp_connection_manager.h"

#ifndef MAXPEERS
#define MAXPEERS 100
#endif

void* pwp_connection_listener(void* foo) {
    jhl_sleep(1.5); // To keep the pace with the main thread.
    
    int listen_fd = jhl_socket_prepare(g_peerport, MAXPEERS);
    if( listen_fd < 0 ) {
        jhl_log("connection_listener: failed to create socket");
        pthread_exit(NULL);
    }
    
    //  jhl_log("connection_listener: listening..");
    puts("************pwp_connection_listener listening.. out of while....*************");
    
    while( !g_done ) {
        puts("************pwp_connection_listener listening.. in while....*************");
        
        int conn_fd;
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        
//        conn_fd = jhl_socket_accept(listen_fd);
        if( conn_fd < 0 ) {
            jhl_log("connection_listener$main_loop: error accepting connection");
            continue;
        } else {
            if( JHL_LOG ) printf("connection_listener$main_loop: accepted connection at %d\n", conn_fd);
        }
        pthread_t thread;
        pwp_arg_t* arg = (pwp_arg_t*) jhl_malloc(sizeof(pwp_arg_t));
        
        arg->socket_fd = conn_fd;
        arg->connecting = 0;
        
        memcpy(arg->ip, inet_ntoa(client_addr.sin_addr), 16);
        arg->port = ntohs(client_addr.sin_port);
        
        pthread_create(&thread, NULL, pwp_connection_manager, arg);
    }
    
    printf("exiting pwp_connection_listener!!!!\n");
    pthread_exit(NULL);
}

#endif
