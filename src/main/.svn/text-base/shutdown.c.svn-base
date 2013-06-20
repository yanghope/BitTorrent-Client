#include "./../util/btdata.h"
#include "./../util/util.h"
#include <stdio.h>

extern int g_done;

void write_to_file(char *filelocation) {
    FILE* f;
    f = fopen(filelocation,"w");
    if( f == NULL ) {
      printf("failure to save file at %s\n", filelocation);
      return;
    }
    fwrite(g_filedata,g_filelen,1,f);
    fclose(f);
    //free(buf);
    printf("file saved\n");
}

// Shut down the client appropriately.
void client_shutdown(int sig)
{
    // Set global stop variable so that we stop trying to connect to peers and
    // allowing connections.  This controls the socket other peers connect to and
    // the thread that connects to other peers.
    g_done = 1;
    int sockfd = -1;
    // int i;
    int mlen;
    char* MESG;
    MESG = make_tracker_request(BT_STOPPED,&mlen);
    
    printf("Creating socket to tracker...in shutdown\n");
    sockfd = connect_to_host(g_tracker_ip, g_tracker_port);
//    puts("socked to tracker created...in shutdown\n");
//    printf("MESG stopped: ");
//    for(i=0; i<mlen; i++)
//        printf("%c",MESG[i]);
//    printf("\n");
    send(sockfd, MESG, mlen, 0);
    puts("stopped message sent!!!!!");

    write_to_file(g_filename);  //todo: make a proper name for this
    
    //free everything
    if (g_current_connections != NULL) {
        free_list(g_current_connections);
    }
    puts("shutdown finished!!!");
}
