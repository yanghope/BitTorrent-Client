
#include "./../util/util.h"
#include "./../util/btdata.h"

// Makes an HTTP request for the tracker.  Uses some global variables to fill 
// info and states stats in, like info_hash, peer_id and how many bytes uploaded 
// and downloaded, etc.
//
// event: 0 - started, 1 - stopped, 2 - completed
// see btdata.h for defines of these quantities
// Function returns message, length of the message is written into mlen
char* make_tracker_request(int event, int* mlen)
{ 
  // Just make a huge message and fill it in
  char *MESG;
  char* cur;
  int i;

  MESG = (char*)malloc(4096*sizeof(char));
  cur = MESG;
  strcpy(cur,"GET /announce?");
  cur += strlen("GET /announce?");
  
  // Fill in info_hash
    strcpy(cur,"info_hash=");
    cur += strlen("info_hash=");
    for(i=0; i<5; i++)
    {
        int j;
        int part = g_infohash[i];
		unsigned char* p = (unsigned char*)&part;
        for(j=0; j<4; j ++)
        {
			*cur++ = '%';
			cur += sprintf(cur, "%02X", p[j]);
            
        }
    }

  // Peer id
  strcpy(cur,"&peer_id=");
  cur += strlen("&peer_id=");
  for(i=0; i<20; i++)
  {
    *cur++ = '%';
    cur += sprintf(cur,"%02X",(unsigned char)g_my_id[i]);
  }
  //strncpy(cur,g_my_id,20);
  //cur += 20;

  // port
  strcpy(cur,"&port=");
  cur += strlen("&port=");
//  cur += sprintf(cur,"%d",g_peerportnum);
    cur += sprintf(cur,"%d",g_peerport);
  
  // ip
  strcpy(cur,"&ip=");
  cur += strlen("&ip=");
  strcpy(cur,g_my_ip);
  cur += strlen(g_my_ip);

  // uploaded
  strcpy(cur,"&uploaded=");
  cur += strlen("&uploaded=");
  cur += sprintf(cur,"%d",g_uploaded);
  
  // downloaded
  strcpy(cur,"&downloaded=");
  cur += strlen("&downloaded=");
  cur += sprintf(cur,"%d",g_downloaded);
  
  // left
  strcpy(cur,"&left=");
  cur += strlen("&left=");
  cur += sprintf(cur,"%d",g_left);

  switch(event) {
  case BT_STARTED:
    strcpy(cur,"&event=");
    cur += strlen("&event=");
    strcpy(cur,"started");
    cur += strlen("started");
    break;
  case BT_STOPPED:
    strcpy(cur,"&event=");
    cur += strlen("&event=");
    strcpy(cur,"stopped");
    cur += strlen("stopped");
    break;
  case BT_COMPLETED:
    strcpy(cur,"&event=");
    cur += strlen("&event=");
    strcpy(cur,"completed");
    cur += strlen("completed");
    break;
  
  // Don't send an event otherwise
  }
  
  // Add footer now
  strcpy(cur," HTTP/1.1\r\n\r\n");
  cur += strlen(" HTTP/1.1\r\n\r\n");

  // And null terminate
  *cur = '\0';

  *mlen = cur - MESG;
  
  return MESG;
}
