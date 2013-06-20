#include "jhl_lib.h"

void jhl_lib_init() {
	
}

void jhl_lib_fin() {
	
}

jhl_list_t* jhl_list_init() {
	jhl_list_t* list = (jhl_list_t*)jhl_malloc(sizeof(jhl_list_t));
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return list;
}

void jhl_list_fin(jhl_list_t* list) {
	if( list->head ) {
		jhl_list_fin_h(list->head);
	}
	jhl_free(list);
}
// Argument node is never NULL.
void jhl_list_fin_h(jhl_list_node_t* node) {
	if( node->next ) {
		jhl_list_fin_h(node->next);
	}
	jhl_free(node);
}

// jhl_list_node_init()
//
// Summary    : Initializes a jhl_list_node.
// Note       : Use jhl_free() to destroy a node.
jhl_list_node_t* jhl_list_node_init(void* data) {
	jhl_list_node_t* node = (jhl_list_node_t*)jhl_malloc(sizeof(jhl_list_node_t));
	node->data = data;
	node->next = NULL;	
	return node;
}

// jhl_list_get_head()
void* jhl_list_get_head(jhl_list_t* list) {
	return list->head->data;
}

// jhl_list_get_tail()
void* jhl_list_get_tail(jhl_list_t* list) {
	return list->tail->data;
}

// jhl_list_get_size()
int jhl_list_get_size(jhl_list_t* list) {
	return list->size;
}

// jhl_list_is_empty()
int jhl_list_is_empty(jhl_list_t* list) {
	return list->size == 0;
}

// jhl_list_add_to_head()
//
// Summary    : Adds new data to the head of jhl_list_t structure.
void jhl_list_add_to_head(jhl_list_t* list, void* data) {
	jhl_list_node_t* new_node = jhl_list_node_init(data);
	if( list-> head ) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		list->head = new_node;
		list->tail = new_node;
	}
	list->size++;
}

// jhl_list_add_to_tail()
//
// Summary    : Adds new data to the tail of jhl_list_t structure.
void jhl_list_add_to_tail(jhl_list_t* list, void* data) {
	jhl_list_node_t* new_node = jhl_list_node_init(data);
	if( list->tail ) {
		list->tail->next = new_node;
		list->tail = new_node;
	} else {
		list->head = new_node;
		list->tail = new_node;
	}
	list->size++;
}

// jhl_list_contains()
//
// Summary   : Checks if the jhl_list_ structure contains data pointed by the argument pointer.
// Arguments : list -- jhl_list_t structure
//             data -- pointer to the data being checked
// Returns   : 1 if data is found.
//             0 otherwise.
int jhl_list_contains_h(jhl_list_node_t* node, void* data);
int jhl_list_contains(jhl_list_t* list, void* data) {
	if( list->head ) {
		return jhl_list_contains_h(list->head, data);
	}
	return 0;
}
// Argument node is never NULL.
int jhl_list_contains_h(jhl_list_node_t* node, void* data) {
	if( node->data == data ) {
		return 1;
	} else if( node->next ) {
		jhl_list_contains_h(node->next, data);
	}
	return 0;
}

// jhl_list_remove_head()
//
// Summary    : Removes the first item of the jhl_list_t structure.
int jhl_list_remove_head(jhl_list_t* list) {
	jhl_list_node_t* temp;
	switch( list->size ) {
		case 0:
			return -1;
		case 1:
			free(list->head);
			list->head = NULL;
			list->tail = NULL;
			break;
		default:
			temp = list->head;
			list->head = list->head->next;
			free(temp);
			break;
	}
	list->size--;
	return 0;
}

// jhl_list_remove_tail()
//
// Summary    : Removes the last item of the jhl_list_t structure.
// Note       : This will take theta(n) time, where n is the size of the list.
int jhl_list_remove_tail(jhl_list_t* list) {
	jhl_list_node_t* temp;
	switch( list->size ) {
		case 0:
			return -1;
		case 1:
			free(list->head);
			list->head = NULL;
			list->tail = NULL;
			break;
		default:
			temp = list->head;
			while( temp->next != list->tail ) {
				temp = temp->next;
			}
			free(list->tail);
			temp->next = NULL;
			list->tail = temp;
	}
	list->size--;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// jhl_stack_t structure                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_stack_t structure
//
// Summary    : Implementation of a simple stack structure.
// Note       : Implementation is done with jhl_list_t.
//typedef jhl_list_t jhl_stack_t;

// jhl_stack_init()
//
// Summary    : Initializes a jhl_stack_t structure.
jhl_stack_t* jhl_stack_init() {
	return (jhl_stack_t*) jhl_list_init();
}

// jhl_stack_fin()
//
// Summary   : Destroys a jhl_stack_t structure.
void jhl_stack_fin(jhl_stack_t* stack) {
	jhl_list_fin((jhl_list_t*) stack);
}

// jhl_stack_push()
//
// Summary   : Stores an item into a jhl_stack_t structure.
void jhl_stack_push(jhl_stack_t* stack, void* data) {
	jhl_list_add_to_head((jhl_list_t*) stack, data);
}

// jhl_stack_pop()
//
// Summary   : Retrieves an item from a jhl_stack_t structure.
//             Also removes it from the structure.
void* jhl_stack_pop(jhl_stack_t* stack) {
	void* popped = jhl_list_get_head((jhl_list_t*) stack);
	jhl_list_remove_head((jhl_list_t*) stack);
	return popped;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// jhl_queue_t structure                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_queue_t structure
//
// Summary    : Implementation of a simple queue structure.
// Note       : Implementation is done with jhl_list_t.
// typedef jhl_list_t jhl_queue_t;

// jhl_queue_init()
//
// Summary    : Initializes a jhl_queue_t structure.
jhl_queue_t* jhl_queue_init() {
	return (jhl_queue_t*) jhl_list_init();
}

// jhl_queue_fin()
//
// Summary   : Destroys a jhl_queue_t structure.
void jhl_queue_fin(jhl_queue_t* queue) {
	jhl_list_fin((jhl_list_t*) queue);
}

// jhl_queue_push()
//
// Summary   : Stores an item into a jhl_queue_t structure.
void jhl_queue_enqueue(jhl_queue_t* queue, void* data) {
	jhl_list_add_to_head((jhl_list_t*) queue, data);
}

// jhl_queue_pop()
//
// Summary   : Retrieves an item from a jhl_queue_t structure.
//             Also removes it from the structure.
void* jhl_queue_dequeue(jhl_queue_t* queue) {
	void* dequeued = jhl_list_get_head((jhl_queue_t*) queue);
	jhl_list_remove_head((jhl_list_t*) queue);
	return dequeued;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// String methods                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_string_brackets()
//
// Summary   : Finds text within brackets (open and close markers).
// Arguments : result      -- string in which parsed result will be stored
//             open        -- open marker  e.g., "("
//             close       -- close marker e.g., ")"
//             string      -- string which will be parsed
//             nested      -- 1 if recognizing nested brackets
//                              0 otherwise (see Examples)
// Condition : Text begins with an open marker and ends with a close marker.
// Result    : result is filled with what is between open and close markers.
// Examples  : result, "(", ")"  , "(x)"    , 0 ==> result becomes "x"
//             result, "(#", "#)", "a(#x#)b", 0 ==> result becomes "x"
//             result, "(", ")"  , "((a))"  , 0 ==> result becomes "(a"
//             result, "(", ")"  , "((a))"  , 1 ==> result becomes "(a)"
// Caveat    : result must be sufficiently long.
// Errors    : When there is no open or close markers.
#define BRACKET_UNOPENED  0
#define BRACKET_OPENED    1
int jhl_string_brackets(char* result, char* open, char* close, char* string, int nested) {
	int open_len   = strlen(open);
	int close_len  = strlen(close);
	int string_len = strlen(string);
	
	int i = 0; // iterator for open marker
	int j = 0; // iterator for close marker
	int m = 0; // iterator for string
	int n = 0; // iterator for parsed
	int level = 0; // level of nested open markers
	int state = BRACKET_UNOPENED;
	
	while( m < string_len ) {
		//printf("%d(%d): %d,%d,%d,%d parsing %c to '%s'\n",
		//	   state, level, i, j, m, n, string[m], result);
		switch( state ) {
			case BRACKET_UNOPENED:
				if( string[m] == open[i] ) {
					if( i == open_len - 1 ) {
						state = BRACKET_OPENED;
						i = 0;
					} else {
						i++;
					}
				} else {
					i = 0;
				}
				break;
			case BRACKET_OPENED:
				if( string[m] == close[j] ) {
					if( j == close_len - 1) {
						if( level ) {
							j = 0;
							level--;
							result[n] = string[m];
							n++;
						} else {
							while( j ) {
								result[n - j] = '\0';
								j--;
							}
							return 0;
						}
					} else {
						result[n] = string[m];
						n++;
						j++;
					}
				} else {
					if( string[m] == close[0] ) {
						j = 1;
					} else {
						j = 0;
					}
					result[n] = string[m];
					n++;
				}
				if( string[m] == open[i] ) {
					if( nested && i == open_len - 1 ) {
						level++;
						i = 0;
					} else {
						i++;
					}
				} else {
					if( string[m] == close[0] ) {
						i = 1;
					} else {
						i = 0;
					}
				}
				break;
			default:
				return -1;
		}
		m++;
	}
	return -2;
}

// jhl_string_shift()
//
// Summary   : Text is shifted forward or backward.
// Arguments : string -- string to be parsed
//             length -- length of string (excluding terminating '\0')
//             n      -- number of chars to shift
//                       positive number to shift forward
//                       negative number to shift backward (see Examples)
// Note      : If shifting forward, eliminated chars are replaced by '\0'.
//             If shifting backward, eliminated chars are replaced by ' '.
//             (See Examples)
// Examples  : "hello", strlen("hello"), 2  ==> string becomes "llo"
//             "hello", strlen("hello"), -2 ==> string becomes "  hel"
int jhl_string_shift(char* string, int length, int n) {
	int i; // iterator for string

	if( n == 0 ) {
		return -1;
	} else if( n > 0 ) {
		i = n;
		while( i < length ) {
			string[i - n] = string[i];
			i++;
		}
		while( i - n < length ) {
			string[i - n] = '\0';
			i++;
		}
	} else {
		n = -n;
		i = length - 1;
		while( i >= n ) {
			string[i] = string[i - n];
			i--;
		}
		while( i >= 0 ) {
			string[i] = ' ';
			i--;
		}
	}
	return 0;
}

// jhl_string_remove()
//
// Summary   : Remove instances of remove marker in string.
// Arguments : string -- string to be parsed
//             remove -- remove marker to be removed from the string
//             once   -- 0 if removing all instances
//                       n > 0 if removing n instances
// Returns   : Number of removed instances.
// Examples  : "hello world", "l",  0 ==> string becomes "heo word"
//             "hello world", "ll", 0 ==> string becomes "heo world"
//             "hello world", "l",  1 ==> string becomes "helo world"
#define REMOVE_MARKER_NOT_FOUND 0
#define REMOVE_MARKER_POSSIBLE  1
int jhl_string_remove(char* string, char* remove, int n) {
	int string_length = strlen(string);
	int remove_length = strlen(remove);
	int i = 0; // iterator for string
	int j = 0; // iterator for remove marker
	int remove_count = 0; // count of removed instances
	int state = REMOVE_MARKER_NOT_FOUND;

	if( n < 0 ) {
		n = 0;
	}
	
	while( i < string_length ) {
		//printf(" %d(%d/%d):%d,%d\t%c in %s\n", state, remove_count, n, i, j, string[i], string);
		switch(state) {
			case REMOVE_MARKER_NOT_FOUND:
				if( string[i] == remove[j] ) {
					j++;
					state = REMOVE_MARKER_POSSIBLE;
				}
				if( j == remove_length ) {
					jhl_string_shift(&string[i - remove_length + 1],
									string_length - i + remove_length, remove_length);
					remove_count++;
					if(remove_count == n) {
						return remove_count;
					}
					i -= remove_length;
					j = 0;
					state = REMOVE_MARKER_NOT_FOUND;
				}
				break;
			case REMOVE_MARKER_POSSIBLE:
				if( string[i] == remove[j] ) {
					j++;
				} else {
					if( string[i] == remove[0] ) {
						j = 1;
					} else {
						j = 0;
						state = REMOVE_MARKER_NOT_FOUND;
					}
				}
				if( j == remove_length ) {
					jhl_string_shift(&string[i - remove_length + 1],
									string_length - i + remove_length, remove_length);
					remove_count++;
					if(remove_count == n) {
						return remove_count;
					}
					i -= remove_length;
					j = 0;
					state = REMOVE_MARKER_NOT_FOUND;
				}
				break;
			default:
				return -2;
		}
		i++;
	}
	return remove_count;
}

// jhl_string_split()
//
// Summary   : Look for marker in string, fill result with text before marker and remainder with
//             text after marker.
// Arguments : result    -- stores text before marker
//             remainder -- stores text after marker
//             string    -- string to be parsed
//             repeat    -- 1 if recognizing multiple markers between result and remainder
//                          0 otherwise (see Examples)
// Returns   : the number of markers between result and remainder
// Examples  : buffer0, buffer1, "..", "1..2", 0
//               ==> buffer0 becomes "1", buffer1 becomes "2", returns 1
//             buffer0, buffer1, "..", "1....2", 1
//               ==> buffer0 becomes "1", buffer1 becomes "2", returns 2
//             buffer0, buffer1, "..", "1....2", 0
//               ==> buffer0 becomes "1", buffer1 becomes "..2", returns 1
#define SPLIT_MARKER_NOT_FOUND 0
#define SPLIT_MARKER_POSSIBLE  1
#define SPLIT_MARKER_MORE      2
#define SPLIT_MARKER_PASSED    3
int jhl_string_split(char* result, char* remainder, char* marker, char* string, int repeat) {
	int string_len = strlen(string);
	int marker_len = strlen(marker);
	
	int i = 0; // iterator for result
	int j = 0; // iterator for remainder
	int k = 0; // iterator for string
	int l = 0; // iterator for marker
	int state = SPLIT_MARKER_NOT_FOUND;
	int split = 0;
	
	while( k < string_len ) {
		//printf(" %d(%d):%d,%d,%d,%d\t'%c' in '%s' into '%s' and '%s'\n",
		//	   state, split, i, j, k, l, string[k], string, result, remainder);
		switch(state) {
			case SPLIT_MARKER_NOT_FOUND:
				if( string[k] == marker[l] ) {
					l++;
					state = SPLIT_MARKER_POSSIBLE;
				}
				if( l == marker_len ) {
					l = 0;
					result[i] = '\0';
					if( repeat ) {
						state = SPLIT_MARKER_MORE;
					} else {
						state = SPLIT_MARKER_PASSED;
					}
					split++;
				} else {
					result[i] = string[k];
					i++;
				}
				break;
			case SPLIT_MARKER_POSSIBLE:
				if( string[k] == marker[l] ) {
					l++;
				} else {
					if( string[k] == marker[0] ) {
						l = 1;
					} else {
						l = 0;
						state = SPLIT_MARKER_NOT_FOUND;
					}
				}
				if( l == marker_len ) {
					while( --l ) {
						result[i - l] = '\0';
					}
					if( repeat ) {
						state = SPLIT_MARKER_MORE;
					} else {
						state = SPLIT_MARKER_PASSED;
					}
					split++;
				} else {
					result[i] = string[k];
					i++;
				}
				break;
			case SPLIT_MARKER_MORE:
				if( string[k] == marker[l] ) {
					l++;
				} else {
					state = SPLIT_MARKER_PASSED;
					split++;
				}
				if( l == marker_len ) {
					l = 0;
					while( j ) {
						remainder[j--] = '\0';
					}
					split++;
				} else {
					remainder[j] = string[k];
					j++;
				}
				break;
			case SPLIT_MARKER_PASSED:
				remainder[j] = string[k];
				j++;
				break;
			default:
				return -2;
		}
		k++;
	}
	remainder[j] = '\0';
	return split;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// File methods                                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_file_read()
//
// Summary   : Read a text file and return pointer to buffer.
// Arguments : file_name   -- name of the file
//             file_length -- pointer to an int (See Modifies)
// Modifies  : file_length -- set to the length of file
// Returns   : Pointer to buffer storing the read text.
// Caveat    : The returned buffer must be freed.
char* jhl_file_read(char* file_name, int* file_length) {
	FILE* fp;
	int file_len;
	char* ptr;
	
	fp = fopen(file_name, "r");
	if(!fp) {
		perror("jhl_lib$jhl_file_read$fopen");
		return NULL;
	}
	if(fseek(fp, 0, SEEK_END) < 0) {
		perror("jhl_lib$jhl_file_read$fseek");
		return NULL;
	}
	file_len = ftell(fp);
	if(fseek(fp, 0, SEEK_SET) < 0) {
		perror("jhl_lib$jhl_file_read$fseek");
		return NULL;
	}
	ptr = jhl_malloc(file_len * sizeof(char));
	if(fread(ptr, file_len, 1, fp) < 0) {
		perror("jhl_lib$jhl_file_read$fread");
		free(ptr);
		return NULL;
	}
	fclose(fp);
	
	*file_length = file_len;
	return ptr;
}

char* jhl_file_read2(char* file_name, int* file_length) {
	FILE* fp;
	int file_len;
	char* ptr;
	
	fp = fopen(file_name, "r+");
	if(!fp) {
		perror("jhl_lib$jhl_file_read$fopen");
		return NULL;
	}
	if(fseek(fp, 0, SEEK_END) < 0) {
		perror("jhl_lib$jhl_file_read$fseek");
		return NULL;
	}
	file_len = ftell(fp);
	if(fseek(fp, 0, SEEK_SET) < 0) {
		perror("jhl_lib$jhl_file_read$fseek");
		return NULL;
	}
	ptr = jhl_malloc(file_len * sizeof(char));
	if(fread(ptr, file_len, 1, fp) < 0) {
		perror("jhl_lib$jhl_file_read$fread");
		free(ptr);
		return NULL;
	}
	fclose(fp);
	
	*file_length = file_len;
	return ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Socket connection methods                                                                      //
////////////////////////////////////////////////////////////////////////////////////////////////////
// A quick overview is as follows:
//
// 1. Server performs the following:
//   - jhl_socket_prepare()
//   - jhl_socket_accept()  <- pass the descriptor returned by jhl_socket_prepare()
//   - (tasks)
//   - jhl_socket_close()   <- pass the descriptor returned by jhl_socket_accept()
//   - jhl_socket_close()   <- pass the descriptor returned by jhl_socket_prepare()
//
// 2. Server for multiple connections calls jhl_prepare() once but jhl_socket_accept() as many times
//    as required.
//
// 3. Client performs the following:
//   - jhl_socket_connect()
//   - (tasks)
//   - jhl_socket_close()   <- pass the descriptor returned by jhl_socket_connect()
//
// 4. jhl_socket_send() and jhl_socket_receive() are shorthands for send() and recv().

// jhl_socket_prepare()
//
// Summary   : Prepares socket connection for servers.
//             Performs socket(), bind(), and listen().
// Arguments : server_port    -- port number
//             connection_max -- maximum number of connections
//                               (if unsure, why not 8?)
// Returns   : file descriptor for the listening socket if successful
//             -1 if socket() fails
//             -2 if bind() fails
//             -3 if connection fails
int jhl_socket_prepare(int server_port, int connection_max) {
	int listen_fd;
	struct sockaddr_in server_addr;
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(server_port);
	
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( listen_fd < 0 ) {
		perror("jhl_lib$jhl_socket_prepare$socket");
		return -1;
	}
	
	if( bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
		perror("jhl_lib$jhl_socket_prepare$bind");
		return -2;
	}
	
	if( listen(listen_fd, connection_max) < 0 ) {
		perror("jhl_lib$jhl_socket_prepare$listen");
		return -3;
	}
	
	return listen_fd;
}

// jhl_socket_accept()
//
// Summary   : Accepts an incoming socket connection request.
//             Performs accept().
// Arguments : listen_fd -- file descriptor for the listening socket
// Returns   : file descriptor for the connected socket if successful
//             -1 if accept() fails
// Note      : This is a blocking function.
int jhl_socket_accept(int listen_fd) {
	int connection_fd;
	struct sockaddr client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	connection_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
	if( connection_fd < 0 ) {
		perror("jhl_lib$jhl_socket_accept$accept");
		return -1;
	}
	
	return connection_fd;
}

// jhl_socket_close()
//
// Summary   : Closes a socket connection.
//             Shorthand for close().
// Arguments : socket_fd -- file descriptor for the socket
// Returns   : 0  if successful
//             -1 if close() fails
int jhl_socket_close(int socket_fd) {
	if( close(socket_fd) < 0 ) {
		perror("jhl_lib$jhl_socket_close$close");
		return -1;
	}
	
	return 0;
}

// jhl_socket_connect()
//
// Summary   : Connects to a listening socket.
//             Performs socket() and connect().
// Arguments : server_IP_address -- the server's IP address
//             server_port       -- the server's port
// Returns   : file descriptor for the connected socket if successful
//             -1 if socket() fails
//             -2 if connect() fails
int jhl_socket_connect(char* server_IP_address, int server_port) {
	int socket_fd;
	struct sockaddr_in server_addr;
	memset(&server_addr, '\0', sizeof(struct sockaddr_in));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(server_IP_address);
	server_addr.sin_port        = htons(server_port);
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if( socket_fd < 0 ) {
		perror("jhl_lib$jhl_socket_connect$socket");
		return -1;
	}
	
//    puts("#################try to connect in jhl_socket connect!!!!!!!!!!###############");
	if( connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0 ) {
		perror("jhl_lib$jhl_socket_connect$connect");
		return -2;
	}
	
	return socket_fd;
}

// jhl_socket_send()
//
// Summary   : Shorthand for send().
// Arguments : socket_fd   -- file descriptor for socket
//             buffer      -- pointer to the buffer to send data
//             buffer_size -- size of buffer
// Returns   : number of bytes of data sent if successful
//             -1 if send() fails
int jhl_socket_send(int socket_fd, char* buffer, size_t buffer_size) {
	int data_sent = send(socket_fd, buffer, buffer_size, 0);
	if( data_sent < 0 ) {
		perror("jhl_lib$jhl_socket_send$send");
		return -1;
	}
	
	return data_sent;
}

// jhl_socket_receive()
//
// Summary   : Shorthand for recv().
// Arguments : socket_fd   -- file descriptor for socket
//             buffer      -- pointer to the buffer to receive incoming data
//             buffer_size -- size of buffer
// Modifies  : buffer      -- will contain the received data
// Returns   : number of bytes of data received if successful
//             0  if connection is closed
//             -1 if recv() fails
int jhl_socket_receive(int socket_fd, char* buffer, size_t buffer_size) {
	int data_received = recv(socket_fd, buffer, buffer_size, 0);
	if( data_received <= 0 ) {
		perror("jhl_lib$jhl_socket_receive$recv");
		return -1;
	}
	
	return data_received;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory methods                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

jhl_malloc_record_t* jhl_malloc_record = NULL;

// jhl_malloc_record_add()
//
// Summary    : Helper method for jhl_malloc().
//              Adds a node to the list of memory-allocated pointers after calling malloc().
void jhl_malloc_record_add(void* ptr) {
	jhl_malloc_record_t* record = jhl_malloc_record;
	// The record list is not empty.
	if( record ) {
		while( record->next ) {
			record = record->next;
		}
		record->next = (jhl_malloc_record_t*)malloc(sizeof(jhl_malloc_record_t));
		if( record->next == NULL ) {
			perror("jhl_lib$jhl_malloc$malloc");
			exit(1);
		} else {
			record->next->ptr  = ptr;
			record->next->next = NULL;
		}
	}
	// The record list is empty.
	else {
		jhl_malloc_record = (jhl_malloc_record_t*)malloc(sizeof(jhl_malloc_record_t));
		if( jhl_malloc_record == NULL ) {
			perror("jhl_lib$jhl_malloc$malloc");
			exit(1);
		} else {
			jhl_malloc_record->ptr  = ptr;
			jhl_malloc_record->next = NULL;
		}
	}
}

// jhl_malloc_record_remove()
//
// Summary    : Helper method for jhl_free().
//              Removes a node from the list of memory-allocated pointers before calling free().
int jhl_malloc_record_remove(void* ptr) {
	jhl_malloc_record_t* record = jhl_malloc_record;
	// The record list is empty.
	if( record == NULL ) {
		printf("jhl_lib$jhl_free: cannot find %p in the list of mallocked pointers\n", ptr);
		return -1;
	}
	// The record list contains one element.
	else if( record->next == NULL ) {
		if( record->ptr == ptr ) {
			free(jhl_malloc_record);
			jhl_malloc_record = NULL;
			return 0;
		} else {
			printf("jhl_lib$jhl_free: cannot find %p in the list of mallocked pointers\n", ptr);
			return -1;			
		}
	}
	// The record list contans multiple elements.
	else {
		// Check the first element.
		if( record->ptr == ptr ) {
			jhl_malloc_record = record->next;
			free(record);
			return 0;
		}
		jhl_malloc_record_t* record_prev = record;
		record = record->next;
		// Check subsequent elements.
		while( record->next ) {
			record_prev = record;
			record = record->next;
			if( record->ptr == ptr ) {
				record_prev->next = record->next;
				free(record);
				return 0;
			}
		}
		// Check the last element.
		if( record->ptr == ptr ) {
			free(record);
			record_prev->next = NULL;
			return 0;
		} else {
			printf("jhl_lib$jhl_free: cannot find %p in the list of mallocked pointers\n", ptr);
			return -1;
		}
	}
	
	// Code should not reach here.
	return -2;
}

// jhl_malloc()
//
// Summary   : Shorthand for memory allocation (malloc()).
//             Performs error check and memset().
// Arguments : size -- amount of memory allocated
// Caveat    : This will exit the program is memory allocation error occurs.
//             The assumption is that the program will run into a fatal error anyway
//             if allocation fails. If failure is addressed, then malloc() should be used.
// Returns   : Pointer to the allocated memory.
void* jhl_malloc(size_t size) {
	void* ptr = malloc(size);
	if(ptr == NULL) {
		perror("jhl_lib$jhl_malloc$malloc");
		exit(1);
	}
	jhl_clean(ptr, size);
	//jhl_malloc_record_add(ptr);
	return ptr;
}

// jhl_free()
//
// Summary   :
// Arguments : ptr -- pointer to the memory allocated
void jhl_free(void* ptr) {
	//if( jhl_malloc_record_remove(ptr) == 0 ) {
		free(ptr);
	//}
}

// jhl_free_all()
//
// Summary   : Frees all memory allocated by jhl_malloc().
void jhl_free_all() {
	jhl_malloc_record_t* record = jhl_malloc_record;
	while( record ) {
		jhl_free( record->ptr );
		record = jhl_malloc_record;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer methods                                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_timer_start()
//
// Summary   : Starts the timer for jhl_time variable.
// Note      : jhl_time is unsigned long int in nanoseconds.
// Usage     : Once timer is started, retrieve the time by calling jhl_timer_get_time().
#define JHL_TIME_INTERVAL 0.1 // in seconds
unsigned long int jhl_time;
pthread_t jhl_time_thread;
pthread_mutex_t jhl_time_lock = PTHREAD_MUTEX_INITIALIZER;
void* jhl_timer_start_h();
void jhl_timer_start() {
	pthread_create(&jhl_time_thread, NULL, jhl_timer_start_h, NULL);
}
void* jhl_timer_start_h() {
	jhl_time = 0L;
	while(1) {
		jhl_sleep(JHL_TIME_INTERVAL);
		pthread_mutex_lock(&jhl_time_lock);
		jhl_time += JHL_TIME_INTERVAL * BILLION;
		pthread_mutex_unlock(&jhl_time_lock);
	}
}

// jhl_timer_get_time()
//
// Summary   : Gets the value of jhl_time variable.
unsigned long int jhl_timer_get_time() {
	unsigned long int retval;
	pthread_mutex_lock(&jhl_time_lock);
	retval = jhl_time;
	pthread_mutex_unlock(&jhl_time_lock);
	return retval;
}

// jhl_timer_get_time()
//
// Summary   : Gets the value of jhl_time variable.
void jhl_timer_set_time(unsigned long int time) {
	pthread_mutex_lock(&jhl_time_lock);
	jhl_time = time;
	pthread_mutex_unlock(&jhl_time_lock);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Comment methods                                                                                //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_log()
//
// Summary   : Prints a message if JHL_LOG == JHL_LOG_ON.
//             This is to reduce efforts in commenting out print statements for debugging.
// Note      : JHL_LOG value can be changed by calls to jhl_log_on() and jhl_log_off().
// Caveat    : jhl_log calls perror() if errno value is set to a non-zero integer. After this call,
//             errno is set back to 0. Check jhl_log_errno to check whether errno was set by the
//             method before jhl_log().
// Examples  : jhl_log(0)$log: message
//             jhl_log(1)$err: message: perror_message
// #define JHL_LOG_OFF 0
// #define JHL_LOG_ON  1
extern int JHL_LOG;
//       = JHL_LOG_ON;
int jhl_log_line  = 0;
int jhl_log_errno = 0;
pthread_mutex_t jhl_log_lock = PTHREAD_MUTEX_INITIALIZER;
void jhl_log(char* message) {
	pthread_mutex_lock(&jhl_log_lock);
	if( JHL_LOG == JHL_LOG_ON ) {
		if( errno ) {
			printf("jhl_log(%d)$err: ", jhl_log_line++); fflush(stdout);
			perror(message);	
			jhl_log_errno = errno;
			errno = 0;
		} else {
			printf("jhl_log(%d)$log: %s\n", jhl_log_line++, message);
		}
	}
	pthread_mutex_unlock(&jhl_log_lock);
}

// jhl_log_on()
//
// Summary    : Enables jhl_log().
// Note       : See jhl_log().
void jhl_log_on() {
	pthread_mutex_lock(&jhl_log_lock);
	JHL_LOG = JHL_LOG_ON;
	pthread_mutex_unlock(&jhl_log_lock);
}

// jhl_log_off()
//
// Summary    : Disables jhl_log().
// Note       : See jhl_log().
void jhl_log_off() {
	pthread_mutex_lock(&jhl_log_lock);
	JHL_LOG = JHL_LOG_ON;
	pthread_mutex_unlock(&jhl_log_lock);
}

// jhl_log_here()
//
// Summary   : Prints a message that stands out. This is for quick debugging purposes, for finding
//             out whether the program reaches a particular line of code or not.
// Note      : Perhaps a good idea is to define a macro with a shorter name.
//             e.g., #define here() jhl_log_here()
#define JHL_LOG_HERE_MESSAGE "\n+--------------+\n|jhl_log_here()|\n+--------------+\n\n"
void jhl_log_here() {
	printf(JHL_LOG_HERE_MESSAGE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Other simple utility methods                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_clean()
//
// Summary   : Shorthand for bzero().
//             jhl_clean(ptr, 0) is shorthand for memset(ptr, '\0', sizeof(ptr)).
//             jhl_clean(ptr, n), where n > 0, is shorthand for memset(ptr, '\0', n).
// Arguments : ptr  -- pointer to object
//             size -- 0 if pointer type is such that sizeof(ptr) suffices.
//                     Otherwise the size of object.
void jhl_clean(void* ptr, size_t size) {
	if( size ) {
		memset(ptr, '\0', size);
	} else {
		memset(ptr, '\0', sizeof(ptr));
	}
}

// jhl_check()
//
// Summary   : If the argument is a negative integer, exits the program.
//             Prints a message upon quitting.
// Arguments : value   -- the integer
//             message -- the message
//                        (some explanation for failure or the method which returned value)
// Example   : -1, "opening file"
//               -> prints "jhl_lib$jhl_check: exiting after failure in opening file" and exits
// Example   : -1, ""
//               -> prints "jhl_lib$jhl_check: exiting" and exits
void jhl_check(int value, char* message) {
	if( value < 0 ) {
		if( message[0] == '\0' ) {
		        printf("jhl_lib$jhl_check(value=%d): exiting\n", value);
		} else {
            		printf("jhl_lib$jhl_check(value=%d): exiting after failure in %s\n", value, message);
		}
		exit(value);
	}
}

// jhl_sleep()
//
// Summary   : Shorthand for nanosleep().
// Arguments : time -- amount of time for sleeping in seconds, in double
//                     i.e., a positive rational number.
int jhl_sleep(double time) {
	struct timespec t1, t2;
	t1.tv_sec = 0;
	while( time >= 1 ) {
		t1.tv_sec++;
		time -= 1;
	}
	t1.tv_nsec = (long) (time * BILLION);
	return nanosleep(&t1, &t2);
}

// jhl_deep_sleep()
//
// Summary   : Infinite loop.
void jhl_deep_sleep() {
	while(1) {
		sleep(KILO);
	}
}
