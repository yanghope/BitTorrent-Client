
#include "./../util/util.h"
#include "./../util/btdata.h"

announce_url_t* parse_announce_url(char* announce)
{
  char* announce_ind;
  char port_str[6];  // Largest number of digits of a port is 5
  int port_len = 0; // Number of chars in port num
  int port;
  char* url;
  int url_len = 0;
  announce_ind = strstr(announce,"/announce");
  announce_ind--;
  while(*announce_ind != ':')
  {
    port_len++;
    announce_ind--;
  }
  strncpy(port_str,announce_ind+1,port_len);
  port_str[port_len+1] = '\0';
  port = atoi(port_str);
    
  // Don't decrement announce_ind b/c we want to stop counting when we hit that
  char* p;
  for(p=announce; p<announce_ind; p++)
  {
    url_len++;   
  }
  /*
  url = (char*)malloc((url_len+1)*sizeof(char)); // +1 for \0
  strncpy(url,announce,url_len);
  url[url_len+1] = '\0';
  */
  announce_url_t* ret;
  ret = (announce_url_t*)malloc(sizeof(announce_url_t));
  if(ret == NULL)
  {
    perror("Could not allocate announce_url_t");
    exit(-73);
  }
 
  p = announce;
  printf("ANNOUNCE: %s\n",announce);
  if(strstr(announce,"http://") > (char*)0)
  {
    //url = url + 7;
    url_len -= 7;
    p += 7;
  }
  
  url = (char*)malloc((url_len+1)*sizeof(char)); // +1 for \0
  //strncpy(url,announce,url_len);
  strncpy(url,p,url_len);
  url[url_len] = '\0';
  
  ret->hostname = url;
  ret->port = port;

  return ret;

}
