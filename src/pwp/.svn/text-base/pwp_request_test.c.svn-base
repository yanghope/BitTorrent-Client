//pwp_request_test.c

#include "pwp_request.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//a char is 8 bits = 1 byte
//12 bits is three chars
//one integer in PWP is a 4-byte big-endian number.
//one integer is 32 bits = 4 bytes
//three integers is 96 bits
// 96 bits is 24 chars

main () {
	// printf("size of int: %d\n", sizeof(int));
	//unsigned char payload[24] = "";
	char* payload = malloc(12*sizeof(char));
	//int index = int_endian_swap(3);
	//int offset = int_endian_swap(10);
	//int size = int_endian_swap(30);
	pwp_request_t r;
	r.index = 131072;
	r.offset = 16383;
	r.length = 2147483644;
	printf("index: %x or %d\n", r.index, r.index);
	printf("offset: %x or %d\n", r.offset, r.offset);
	printf("size: %x or %d\n", r.length, r.length);
	/*
	payload[0] = 'a';
	payload[0] = 0x20; //gives 32
	payload[1] = 0x101; //gives 1
	printf("payload[0]: %x or %d \n", payload[0], payload[0]);
	printf("payload[1]: %x or %d\n", payload[1], payload[1]);
	*/
	/*memcpy(payload+0, &index, 4);
	memcpy(payload+4, &offset, 4);
	memcpy(payload+8, &size, 4);
	*/
	make_request_payload(r, payload);
	int ret_index, ret_offset, ret_size;
	ret_index = get_request_piece_index(payload);
	ret_offset = get_request_offset(payload);
	ret_size = get_request_length(payload);
	/*
	ret_index = get_request_piece_index(payload);
	ret_offset = get_request_offset(payload);
	ret_size = get_request_length(payload);
	*/
	printf("ret index: %x or %d\n",ret_index, ret_index);
	printf("ret offset: %x or %d\n",ret_offset, ret_offset);
	printf("ret size: %x or %d\n",ret_size, ret_size);;
 	free(payload);

	int i = 1;
   	char *c = (char*)&i;
   	if (*c)    
   	    printf("Little endian\n");
   	else
    	printf("Big endian\n");

   	return 0;
}