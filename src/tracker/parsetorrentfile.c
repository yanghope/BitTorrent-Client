
#include "./../util/bencode.h" /* JHL. */
#include "./../util/util.h"    /* I made a hack change to line 112 to handle the issue where */
#include "./../util/sha1.h"    /* idict[5].key would have the value 0x60 (no idea why) */
//#include <openssl/sha.h>

// Note: this function only handle single file torrents
torrentmetadata_t* parsetorrentfile(char* filename)
{
    int i;
    be_node* ben_res;
    FILE* f;
    int flen;
    char* data;
    torrentmetadata_t* ret;
    
    // Open file, get data and decode the string
    f = fopen(filename,"r"); if(!f) {perror("parsetorrentfile$fopen"); return NULL;}
    flen = file_len(f);
    data = (char*)malloc(flen*sizeof(char));
    fread((void*)data,sizeof(char),flen,f);
    fclose(f);
    ben_res = be_decoden(data,flen);
    
    // Go through the nodes, check structure of file and fill in necessary
    // fields.
    if(ben_res->type != BE_DICT)
    {
        perror("Torrent file isn't a dictionary");
        exit(-13);
    }
    
    ret = (torrentmetadata_t*)malloc(sizeof(torrentmetadata_t));
    if(ret == NULL)
    {
        perror("Could not allocate torrent meta data");
        exit(-13);
    }
    
    // Determine info_hash value for this torrent
    // FYI:
    // The SHA1 function returns the hash value as an array of ints, which on
    // little endian machines will need to be reversed when comparing the hash
    // values returned from the tracker and other peers over the wire.  Also,
    // when you generate responses for the tracker you'll have to reverse the
    // bytes.
    char* info_loc, *info_end;
    info_loc = strstr(data,"infod");  // look for info that is key with val a dictionary
    info_loc += 4; // move pointer to where the 'value' starts
    info_end = data+flen-1;
    info_end--;
    // Get rid of trailing e's
//    while(*info_end == 'e')
//        --info_end;
//    // Add one back b/c it has to close the dict, this would be much easier if we
//    // could rebencode a node, someone may want to write this at some point
//    ++info_end;
//    *info_end = 'e';
    
    char*p;
    int len = 0;
    for(p=info_loc; p<=info_end; p++) len++;
    
    // Compute SHA1 hash of above string to get info_hash
    SHA1Context sha;
    SHA1Reset(&sha);
    unsigned char Message_Digest[20];
    SHA1Input(&sha,(const unsigned char*)info_loc,len);
    if(!SHA1Result(&sha, Message_Digest))
    {
        printf("FAILURE in SHA1Result\n");
    }
    memcpy(ret->info_hash,Message_Digest,20);
    
//    SHA1((const unsigned char*)info_loc, len, (unsigned char*)(ret->info_hash));
    
//    printf("SHA1:\n");
//    for(i=0; i<5; i++)
//    {
//        //printf("%08X ",(sha.Message_Digest[i]));
//        printf("%08X ",ret->info_hash[i]);
//    }
//    printf("\n");
    /*
     p = (char*)ret->info_hash;
     for(i=0; i<20; i++)
     {
     //printf("%08X ",(sha.Message_Digest[i]));
     printf("%02X ",(unsigned char)*(p+i));
     }
     printf("\n");
     */
    
    // Go through keys and extract relevant information
    int filled=0;
    for(i=0; ben_res->val.d[i].val != NULL; i++)
    {
        int j;
        if(!strncmp(ben_res->val.d[i].key,"announce",strlen("announce")))
        {
            char* url = (char*)ben_res->val.d[i].val->val.s;
//            printf("URL: %s\n",url);
//            ret->announce = url;
            ret->announce = (char *)malloc(strlen(url) * sizeof(char));
            memcpy(ret->announce, url, strlen(url));
            filled++;
        }
        // info is a dict with some other keys we care about
        if(!strncmp(ben_res->val.d[i].key,"info",strlen("info")))
        {
            be_dict* idict;
            if(ben_res->val.d[i].val->type != BE_DICT)
            {
                perror("Expected dict, got something else");
                exit(-3);
            }
            idict = ben_res->val.d[i].val->val.d;
            
            // Go through keys of this dict -- the keys in the file will probab
            for(j=0; idict[j].key != NULL && idict[j].key != (void*)0x60; j++)
            {
                //printf("Key: %s\n",idict[j].key);
                
                if(!strncmp(idict[j].key,"length",strlen("length")))
                {
                    //printf("Key: %s, Val: %d\n",idict[j].key,idict[j].val->val.i);
                    ret->length = idict[j].val->val.i;
                    filled++;
                }
                if(!strncmp(idict[j].key,"name",strlen("name")))
                {
                    //printf("Key: %s, Val: %s\n",idict[j].key,idict[j].val->val.s);
                    ret->name = (char*)malloc(strlen(idict[j].val->val.s)*sizeof(char));
                    memcpy(ret->name,idict[j].val->val.s,strlen(idict[j].val->val.s));
                    //ret->name = idict[j].val->val.s;
                    filled++;
                }
                if(!strncmp(idict[j].key,"piece length",strlen("piece length")))
                {
                    //printf("Key: %s, Val: %d\n",idict[j].key,idict[j].val->val.i);
                    ret->piece_len = idict[j].val->val.i;
                    filled++;
                }
                if(!strncmp(idict[j].key,"pieces",strlen("pieces")))
                {
//                    printf("Key: %s, Val: %s\n",idict[j].key,idict[j].val->val.s);
                    int num_pieces = ret->length/ret->piece_len;
                    if(ret->length % ret->piece_len != 0)
                        num_pieces++;
                    ret->pieces = (char*)malloc(num_pieces*20);
                    memcpy(ret->pieces,idict[j].val->val.s,num_pieces*20);
                    ret->num_pieces = num_pieces;
                    //ret->pieces = idict[j].val->val.s;
                    filled++;
                }
                
                // Get SHA1 hash of bencoded string of the value for info
                
            } // End for
        } // End info key processing
    }
    
    // Make sure we filled necessary fields
 
    be_free(ben_res);
    
    if(filled < 5)
    {
        printf("Did not fill necessary field\n");
        return NULL;
    }
    else
//        puts("!!!");
        return ret;
}
