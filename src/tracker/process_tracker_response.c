
#include "./../util/btdata.h"
#include "./../util/util.h"

// Read and process the HTTP response from a tracker and make sure it's good
// and extract the data from it.  A tracker HTTP response looks like this:
// (I've annotated it)
// HTTP/1.0 200 OK       (17 chars)
// Content-Length: X     (16 chars to 1st space) Note: X is a number here
// Content-Type: text/plain (26 chars)
// Pragma: no-cache (18 chars)
// \r\n  (This is an extra line so we know where data starts)
// data                  Note: data is just the data, there's no data tag
tracker_response* preprocess_tracker_response(int sockfd)
{
    char rcvline[MAXLINE];
    char tmp[MAXLINE];
    char* data;
    int len;
    //int offset = 0;
    int datasize = -1;
    //printf("Reading tracker response...\n");
    // HTTP LINE
    len = recv(sockfd,rcvline,17,0);
    if(len < 0)
    {
        perror("Error, cannot read socket from tracker");
        exit(-6);
    }
    strncpy(tmp,rcvline,17);
    
//    int l;
//    for(l=0; l<100; l++)
//      printf("%c",tmp[l]);
//    printf("\n");
    
    if(strncmp(tmp,"HTTP/1.1 200 OK\r\n",strlen("HTTP/1.1 200 OK\r\n")))  //XNH: Now the tracker sends HTTP/1.1 back. It was HTTP/1.0 previously.
    {
        perror("Error, didn't match HTTP line");
        exit(-6);
    }
    memset(rcvline,0xFF,MAXLINE);
    memset(tmp,0x0,MAXLINE);
    
    //Read in Content-type
    len = recv(sockfd,rcvline,26,0);
    if(len <= 0)
    {
        perror("Error, cannot read socket from tracker");
        exit(-6);
    }
    
    // Content-Length
    len = recv(sockfd,rcvline,16,0);
    if(len <= 0)
    {
        perror("Error, cannot read socket from tracker");
        exit(-6);
    }
    strncpy(tmp,rcvline,16);
//    puts(tmp);
    if(strncmp(tmp,"Content-Length ",strlen("Content-Length")))
    {
        perror("Error, didn't match Content-Length line");
        exit(-6);
    }
    memset(rcvline,0xFF,MAXLINE);
    memset(tmp,0x0,MAXLINE);
    // Now read in data size
    char c[2];
    char num[MAXLINE];
    int count = 0;
    c[0] = 0; c[1] = 0;
    while(c[0] != '\r' && c[1] != '\n')
    {
        len = recv(sockfd,rcvline,1,0);
        if(len <= 0)
        {
            perror("Error, cannot read socket from tracker");
            exit(-6);
        }
        num[count] = rcvline[0];
        c[0] = c[1];
        c[1] = num[count];
        count++;
    }
    datasize = atoi(num);
//    printf("DATASIZE RECEIVED: %d\n",datasize);
    memset(rcvline,0xFF,MAXLINE);
    memset(num,0x0,MAXLINE);
    
    // Read in Pragma lines
//    len = recv(sockfd,rcvline,18,0);
//    if(len <= 0)
//    {
//        perror("Error, cannot read socket from tracker");
//        exit(-6);
//    }
    
    // Get rid of extra \r\n line in response
    len = recv(sockfd,rcvline,2,0);
    if(len <= 0)
    {
        perror("Error, cannot read socket from tracker");
        exit(-6);
    }
    
    // Now allocate space for and read the data, include space for \0 char
    // b/c need this to be a string
    int i;
    data = (char*)malloc((datasize+1)*sizeof(char));
    for(i=0; i<datasize; i++)
    {
        len = recv(sockfd,data+i,1,0);
        if(len < 0)
        {
            perror("Error, cannot read socket from tracker");
            exit(-6);
        }
        if(len > 1)
        {
            perror("Error: read more than 1 byte of data...");
            exit(-13);
        }
    }
    data[datasize] = '\0';
    
//    for(i=0; i<datasize; i++)
//        printf("%c",data[i]);
//    printf("\n");
    
    // Allocate and return tracker_response struct with fields filled in.
    tracker_response* ret;
    ret = (tracker_response*)malloc(sizeof(tracker_response));
    if(ret == NULL)
    {
        printf("Error allocating tracker_response ptr\n");
        return 0;
    }
    ret->size = datasize;
    ret->data = data;
    
    return ret;
}

// Decode the bencoded data and parse into tracker data structure
tracker_data* get_tracker_data(char* data, int len)
{
    tracker_data* ret;
    be_node* ben_res;
    ben_res = be_decoden(data,len);
    if(ben_res->type != BE_DICT)
    {
        perror("Data not of type dict");
        exit(-12);
    }
    
    ret = (tracker_data*)malloc(sizeof(tracker_data));
    if(ret == NULL)
    {
        perror("Could not allcoate tracker_data");
        exit(-12);
    }
    
    // Go through keys and test them
    int i;
    for (i=0; ben_res->val.d[i].val != NULL; i++)
    {
//        printf("type is %d, %s\n", ben_res->val.d[i].val->type, ben_res->val.d[i].key);
        // Check if failure key
        if(!strncmp(ben_res->val.d[i].key,"failure reason",strlen("failure reason")))
        {
            printf("Error: %s",ben_res->val.d[i].val->val.s);
            exit(-12);
        }
        // interval key
        if(!strncmp(ben_res->val.d[i].key,"interval",strlen("interval")))
        {
            ret->interval = (int)ben_res->val.d[i].val->val.i;
            printf("type is %d, %s: %d\n",ben_res->val.d[i].val->type, ben_res->val.d[i].key, ret->interval);
        }
        // peers key
        if(!strncmp(ben_res->val.d[i].key,"peers",strlen("peers")))
        {
//            printf("HAVE A PEER KEY\n");
            
            be_node* peer_list = ben_res->val.d[i].val;
//            printf("CHECK: %d\n",peer_list->type);
//            printf("CHECK: %d\n",ben_res->val.d[i].val->type);
//            printf("%d\n",peer_list->val.l[0]->type);
            get_peers(ret,peer_list);
        }
    }
    
    be_free(ben_res);
    
    return ret;
}
// Process list of peers in dictionary from tracker. Dictionary version
//void get_peers(tracker_data* td, be_node* peer_list)
//{
//    int i;
//    int numpeers = 0;
//    
//    // Count number of peers in list
//    for (i=0; peer_list->val.l[i] != NULL; i++)
//    {
//        // Make sure element is a dict
//        printf("get_peers, %d\n", i);
//        if(peer_list->val.l[i]->type != BE_DICT)
//        {
//            perror("Expecting dict, got something else");
//            exit(-12);
//        }
//        // Found info for a peer, increase number of them
//        numpeers++;
//    }
//    
//    printf("Num peers: %d\n",numpeers);
//    
//    // Allocate space for peers
//    td->numpeers = numpeers;
//    td->peers = (peerdata*)malloc(numpeers*sizeof(peerdata));
//    if(td->peers == NULL)
//    {
//        perror("Couldn't allocate peers");
//        exit(-12);
//    }
//    
//    // Get data for each peer
//    for (i=0; peer_list->val.l[i] != NULL; i++)
//    {
//        // Make sure element is a dict
//        if(peer_list->val.l[i]->type != BE_DICT)
//        {
//            perror("Expecting dict, got something else");
//            exit(-12);
//        }
//        
//        // Found info for a peer, increase number of them
//        get_peer_data(&(td->peers[i]),peer_list->val.l[i]);
//    }
//    
//    return;
//}

// Process list of peers in dictionary from tracker. String version
void get_peers(tracker_data* td, be_node* peer_list)
{
    int i;
    
    int numpeers = (peer_list->len - 1)/6;
    
//    printf("in get_peers, numpeers: %d\n", numpeers);
    // Allocate space for peers
    td->numpeers = numpeers;
    td->peers = (peerdata*)malloc(numpeers * sizeof(peerdata));
    if(td->peers == NULL)
    {
        perror("Couldn't allocate peers");
        exit(-12);
    }
    
    // Get data for each peer
	struct in_addr in;
	char port[2];
    for (i=0; i < numpeers; i++)
    {
		memset(&(td->peers[i]), 0, sizeof(peerdata));
		memcpy(&in, peer_list->val.s+(i*6), 4);
        td->peers[i].ip = (char *)malloc(strlen(inet_ntoa(in)));
		strcpy(td->peers[i].ip, inet_ntoa(in));
		memcpy(port, peer_list->val.s+(i*6)+4, 2);
		td->peers[i].port = ntohs(*((unsigned short int*)port));
    }
    
    return;
}

// Given ptr of a peerdata and a peer's dictionary of
// relevant data, fill out the structure.
void get_peer_data(peerdata* peer, be_node* ben_res)
{
    int i;
    
    if(ben_res->type != BE_DICT)
    {
        perror("Don't have a dict for this peer");
        exit(-12);
    }
    
    // Got through keys and fill in the peerdata structure
    for (i=0; ben_res->val.d[i].val != NULL; i++)
    {
        //printf("%s\n",ben_res->val.d[i].key);
        
        // Check if peer id
        if(!strncmp(ben_res->val.d[i].key,"peer id",strlen("peer id")))
        {
            //printf("Peer id: %s\n", ben_res->val.d[i].val->val.s);
            memcpy(peer->id,ben_res->val.d[i].val->val.s,20);
            peer->id[20] = '\0';
            /*
             int idl;
             printf("Peer id: ");
             for(idl=0; idl<len; idl++)
             printf("%02X ",(unsigned char)peer->id[idl]);
             printf("\n");
             */
        }
        // ip key
        if(!strncmp(ben_res->val.d[i].key,"ip",strlen("ip")))
        {
            int len;
            //printf("Peer ip: %s\n",ben_res->val.d[i].val->val.s);
            len = strlen(ben_res->val.d[i].val->val.s);
            peer->ip = (char*)malloc(len*sizeof(char));
            strcpy(peer->ip,ben_res->val.d[i].val->val.s);
        }
        // port key
        if(!strncmp(ben_res->val.d[i].key,"port",strlen("port")))
        {
            //printf("Peer port: %d\n",ben_res->val.d[i].val->val.i);
            peer->port = ben_res->val.d[i].val->val.i;
        }
    }
}
