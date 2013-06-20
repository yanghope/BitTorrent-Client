#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "./../util/util.h"
#include "./../util/btdata.h"
#include "./../util/bencode.h"
#include "./../pwp/pwp_connection_manager.h"

//#define MAXLINE 4096 /*max text line length*/

// pthread data
void init()
{
    g_done = 0;
    g_tracker_response = NULL;
    g_current_connections = NULL;
    g_current_connections_number = 0;
    g_filedata = NULL;
    g_peerport = 5566;
    g_uploaded = 0;
    g_downloaded = 0;
    pthread_mutex_init(&g_tracker_report_variables_mutex, NULL);
}

void* simple_timer(void*);
int main(int argc, char **argv)
{
    int sockfd = -1;
    char rcvline[MAXLINE];
    char tmp[MAXLINE];
    // FILE* f;
    // int rc;
    int i;
    
    // NOTE:
    // You can change the usage of this program to be however you would like.
    if(argc < 4)
    {
        printf("Usage: DartTorrent <torrent file> <ip of this machine (XXX.XXX.XXX.XXX form)> <downloaded file location> [isseed]\n");
        printf("\t isseed is optional, 1 indicates this is a seed and won't contact other clients\n");
        printf("\t 0 indicates a downloading client and will connect to other clients and receive connections\n");
        exit(-1);
    }
    
    int iseed = 0;
    if( argc > 4 ) {
        iseed = !!atoi(argv[4]);
    }
    
    if( iseed ) {
        printf("DartTorrent running as seed..\n");
    }
    init();
    srand(time(NULL));
    
    int val[5];
    for(i=0; i<5; i++)
    {
        val[i] = rand();
    }
    
    memcpy(g_my_id,(char*)val,20);
    strncpy(g_my_ip,argv[2],strlen(argv[2]));
    //strncpy(g_filename, argv[3], strlen(argv[3]));
    g_filename = argv[3];
    
    g_torrentmeta = parsetorrentfile(argv[1]);
    memcpy(g_infohash,g_torrentmeta->info_hash,20);
    
    g_filelen = g_torrentmeta->length;
    g_left = g_torrentmeta->length;
    g_num_pieces = g_torrentmeta->num_pieces;
    
    announce_url_t* announce_info;
    puts(g_torrentmeta->announce);
    announce_info = parse_announce_url(g_torrentmeta->announce);
    
    // Get ip address of tracker url
    printf("HOSTNAME: %s\n",announce_info->hostname);
    struct hostent *record;
    record = gethostbyname(announce_info->hostname);
    if (record==NULL)
    {
        printf("gethostbyname(%s) failed", announce_info->hostname);
        exit(1);
    }
    struct in_addr* address;
    address =(struct in_addr * )record->h_addr_list[0];
    //    printf("address: %d\n",*address);
    printf("Tracker IP Address: %s\n", inet_ntoa(* address));
    strcpy(g_tracker_ip,inet_ntoa(*address));
    g_tracker_port = announce_info->port;
    
    free(announce_info);
    announce_info = NULL;
    
    if(pthread_mutex_init(&g_tracker_response_mutex,NULL) != 0)  //XNH: Here, init function returns 0 if success
    {
        printf("Could not make mutex\n");
        exit(-3);
    }
    
    // Initialization
    // Set up signal handler
    signal(SIGINT,client_shutdown);
    
    // Set up thread to listen for peers on
    
    // g_filedata
    // No action taken at the moment.
    if( iseed ) {
        int file_length = -1;
        g_filedata = jhl_file_read(argv[3], &file_length);
//        g_left = 0;
        if( file_length != g_torrentmeta->length ) {
            printf("main: file length (%d) is different from file length described in the metainfo file (%d)\n",
                   file_length, g_torrentmeta->length);
            exit(-1);
        }
    } else {
        int file_length = -1;
        if (!(g_filedata = jhl_file_read2(argv[3], &file_length))) {
            g_filedata = (char*)malloc(g_filelen*sizeof(char));
            jhl_clean(g_filedata, g_filelen * sizeof(char));
            puts("New downloader! No such file, will create after download...");
        }
        else if( file_length != g_torrentmeta->length ) {
            printf("main: file length (%d) is different from file length described in the metainfo file (%d)\n",
                   file_length, g_torrentmeta->length);
            exit(-1);
        }
//        jhl_clean(g_filedata, g_filelen * sizeof(char));
    }
    
    // Contact tracker every so often
    int firsttime = 1;
    int mlen;
    char* MESG;
    MESG = make_tracker_request(BT_STARTED,&mlen);
    puts(MESG);
    
    //--------------------------------------------------------------------------------
    // JHL. Notes about global variables.
    // g_filedata
    //   - Mallocked but it does not read any file at from any location.
    //   - If there are available pieces of the file, it should read.
    //--------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------
    // JHL. Declaring global variables.
    
    // Socket connecting address.
    
    // Bitfield (g_bitfield)
    bitf_global_init();
    if (g_left == 0) {;
        puts("g_left is set to 0, I'm a seeder!!");
    }
    
    // jhl_lib variables
    //    jhl_timer_start();
    //    pthread_t timer_thread;
    //    pthread_create(&timer_thread, NULL, simple_timer, NULL);
    jhl_log_on();
    // PWP connections
    jhl_clean(g_pwp_connections, MAX_PWP_CONNECTIONS * sizeof(pwp_arg_t));
    for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
        g_pwp_connections[i].socket_fd = -1;
    }
    pthread_mutex_init(&g_pwp_connections_mutex, NULL);
    
    // PWP connection listener thread.
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, pwp_connection_listener, NULL);
    // The thread will start in 3 seconds.
    
    // JHL. Declaring global variables. End.
    //--------------------------------------------------------------------------------
    
    while(!g_done)
    {
        //rl: quick hack: write the dling file to file every iteration of this.
        //todo: make a new thread to write the file every ? seconds?
        //write_to_file("torrent_recv"); //todo: make a proper name for this
//        write_to_file(g_filename);
        
        if(sockfd <= 0)
        {
            //Create a socket to send to tracker
            //printf("Creating socket to tracker...%s\t%d\n", g_tracker_ip, g_tracker_port);
            //sockfd = connect_to_host(TRACK_IP, TRACK_PORT);
            sockfd = connect_to_host(g_tracker_ip, g_tracker_port);
        }
        
        //printf("Sending request to tracker...\n");
        
        // Actually sending message
        if(!firsttime)
        {
            free(MESG);
            // -1 specifies no event
            if( !iseed && g_left == 0 ) {
                MESG = make_tracker_request(BT_COMPLETED, &mlen);
                printf("-------------To tracker: I completed the download!!!!!\n");
            } else {
                MESG = make_tracker_request(-1,&mlen);
            }
        }
        
        send(sockfd, MESG, mlen, 0);
        firsttime = 0;
        
        memset(rcvline,0x0,MAXLINE);
        memset(tmp,0x0,MAXLINE);
        
        // Read and process tracker response
        if(sockfd <= 0)
        {
            //Create a socket to send to tracker
            //printf("Creating socket to tracker...again\n");
            //sockfd = connect_to_host(TRACK_IP, TRACK_PORT);
            sockfd = connect_to_host(g_tracker_ip, g_tracker_port);
        }
        tracker_response* tr;
        tr = preprocess_tracker_response(sockfd); // This function actually reads off the socket
        
        // Shutdown and close the socket so we can use it again
        shutdown(sockfd,SHUT_RDWR);
        close(sockfd);
        sockfd = 0;
        
        //printf("Decoding response...\n");
        // probably don't need tmp2 through here
        char* tmp2 = (char*)malloc(tr->size*sizeof(char));
        memcpy(tmp2,tr->data,tr->size*sizeof(char));
        
        //        printf("%c,%c,%c,%c\n", tmp2[94], tmp2[95], tmp2[96], tmp2[97]);
        //        for(i=0; i<tr->size; i++)
        //            printf("%c",tmp2[i]);
        //        printf("\n");
        
        //printf("Parsing tracker data\n");
        // This looks wrong but is right
        pthread_mutex_lock(&g_tracker_response_mutex);
        g_tracker_response = get_tracker_data(tmp2,tr->size);
        pthread_mutex_unlock(&g_tracker_response_mutex);
        
        //if(tmp)
        //{
        free(tmp2);
        tmp2 = NULL;
        //}
        
        //        printf("Num Peers: %d\n",g_tracker_response->numpeers);
        //        for(i=0; i<g_tracker_response->numpeers; i++)
        //        {
        //            printf("Peer id: %s\n",g_tracker_response->peers[i].id);
        //            printf("Peer id: ");
        //            int idl;
        //            for(idl=0; idl<20; idl++)
        //                printf("%02X ",(unsigned char)g_tracker_response->peers[i].id[idl]);
        //            printf("\n");
        //            printf("Peer ip: %s\n",g_tracker_response->peers[i].ip);
        //            printf("Peer port: %d\n",g_tracker_response->peers[i].port);
        //        }
        
        if( !iseed && g_left > 0) {
            //create the new peers list for PWP to connect
            remote_peer_t *new_peers_list = get_new_peers(g_tracker_response);
            
            //--------------------------------------------------------------------------------
            // JHL. Calling connection manager threads.
            //printf("main$connect_loop: entering\n");
            
            
            
            int index = 0;
            remote_peer_t* iter = new_peers_list;
            while (iter) {
                printf("--Peer ip: %s, port: %d\n",iter->ip, iter->port);
                iter = iter->next;
            }
            iter = new_peers_list;
            
            while( iter ) {
                // Check for duplicacy.
                // puts("in loop!!!!");
                int duplicate = 0;
                
                // I should not connect to myself.
                if( strcmp(iter->ip, g_my_ip) == 0 && iter->port == g_peerport ) {
                    printf("main$connection_loop: peer %d is at the same IP as myself\n", index);
                    iter = iter->next;
                    index++;
                    continue;
                }
                
                //puts("waiting for lock!!!");
                pthread_mutex_lock(&g_pwp_connections_mutex);
                //puts("has lock!!!");
                for(int i = 0; i < MAX_PWP_CONNECTIONS; i++) {
                    if( g_pwp_connections[i].socket_fd >= 0 &&
                       strcmp(iter->ip, g_pwp_connections[i].ip) == 0 &&
                       iter->port == g_pwp_connections[i].port) {
                        printf("************main$connection loop: peer %d is duplicate*************\n", index);
                        duplicate = 1;
                        break;
                    }
                }
                
                pthread_mutex_unlock(&g_pwp_connections_mutex);
                if( duplicate ) {
                    iter = iter->next;
                    index++;
                    continue;
                }
                
                // I should connect to this peer.
                else {
                    pwp_arg_t* a = (pwp_arg_t*) jhl_malloc(sizeof(pwp_arg_t));
                    a->socket_fd = -1;
                    memcpy(a->ip, iter->ip, 16);
                    a->port = iter->port;
                    a->connecting = 1;
                    pthread_t thread;
                    //printf("main$connection_loop$creating peer %d\n", index);
                    pthread_create(&thread, NULL, pwp_connection_manager, a);
                }
                
                // Iterate.
                iter = iter->next;
                index++;
            }
            
            sleep(1); // For threads to copy arguments.
        //printf("main$connect_loop: fin\n");
        // JHL. Calling connection managers threads. End.
        //--------------------------------------------------------------------------------
        
        //Testing ************
        //        if (g_current_connections == NULL) {
        //            puts("g_current_connections INITIALIZING!");
        //            g_current_connections = new_peers_list;
        //            g_current_connections_number = g_tracker_response->numpeers;
        //        }
        //        puts("*************");
        //        if (!add_to_list(&g_current_connections, new_peers_list)) {
        //            puts("Add to list fails!");
        //        }
        //        add_to_list(&g_current_connections, new_peers_list);
        //        remote_peer_t *cur = g_current_connections;
        //        while (cur != NULL) {
        //            printf("Peer ip: %s\n",cur->ip);
        //            printf("Peer port: %d\n",cur->port);
        //            cur = cur->next;
        //        }
        //        puts("$$$$$$$$$$$$$$");
        //
        //
        //
        //        if(!remove_from_list(&g_current_connections, new_peers_list)) {
        //            puts("Remove from list fails!");
        //        }
        //        cur = g_current_connections;
        //        while (cur != NULL) {
        //            printf("Peer ip: %s\n",cur->ip);
        //            printf("Peer port: %d\n",cur->port);
        //            cur = cur->next;
        //        }
        //        puts("*************");
        
        //**************
            
            
            
            // Must sleep for td->interval seconds before issuing another GET request
            if (new_peers_list != NULL) {
                free_list(new_peers_list);
            }
        }
        //        printf("sleeping for %d seconds\n\n", g_tracker_response->interval / 250);
        sleep(g_tracker_response->interval / 250);        
    }
    
    // Sleep so other threads can close their sockets, as only get here after
    // ctrl-c
    sleep(2);
    
    exit(0);
}

void* simple_timer(void* _) {
    int seconds = 0;
    while(1) {
        printf("simple_timer: running for %d seconds..\n", seconds);
        sleep(10);
        seconds += 10;
    }
}
