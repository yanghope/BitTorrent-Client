// jhl_lib.h
//
// Author    : Jae Heon Lee
//             cs.dartmouth.edu/~jhl
//
// Date      : 2013-05-22
//
// Summary   : A collection of simple utility mehtods for personal purposes.
// Namespace : jhl_*
// Note      : This header file includes all code; there is no jhl_lib.c file.
// Usage     : Include this header in .c files.
//             Compile with -lpthread switch.
// 
// Caveat    : The code was written and tested in the following environment:
//             OS       -- Mac OS X Version 10.5.8
//             Editor   -- Xcode Version 3.1.4
//             Compiler -- gcc
//             Flags    -- -std=c99 -Wall -pedantic -lpthread

// WARNING
// For CS 60 Project Team
//
// Many methods have errors! In particular, don't use data structures.

////////////////////////////////////////////////////////////////////////////////////////////////////
// The above 100 sleshes are for controlling margins (for aesthetic purposes).

#ifndef _JHL_LIB_H
#define _JHL_LIB_H

// C standard header files inlcuded.
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// Large number constants
#define THOUSAND 1000
#define MILLION  1000000
#define BILLION  1000000000
#define TRILLION 1000000000000
#define KILO     1024
#define MEGA     1048576
#define GIGA     1073741824
#define TERA     1099511627776

// Method declarations for methods used within this file.
void* jhl_malloc (size_t size);
void  jhl_free   (void* ptr);
void  jhl_clean  (void* ptr, size_t size);
int   jhl_sleep  (double time);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Library initialization and finalization                                                        //
////////////////////////////////////////////////////////////////////////////////////////////////////

void jhl_lib_init();
void jhl_lib_fin();


////////////////////////////////////////////////////////////////////////////////////////////////////
// jhl_list_t structure                                                                           //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_list_t structure
//
// Summary   : Implementation of simple singly-linked list data structure.
//             (1) jhl_list_t
//               - Maintains pointers to the first and last items of the list.
//               - Maintains the size of the list.
//             (2) jhl_list_node_t
//               - Maintains pointer to the data.
//               - Contains pointer to the next node (jhl_list_node_t*).
// Note      : Use jhl_list_init() to initialize and jhl_list_fin() to destroy lists.
typedef struct jhl_list jhl_list_t;
typedef struct jhl_list_node jhl_list_node_t;
struct jhl_list {
	int size;
	jhl_list_node_t* head;
	jhl_list_node_t* tail;
};
struct jhl_list_node {
	void* data;
	jhl_list_node_t* next;
};

// jhl_list_init()
//
// Summary    : Initializes a jhl_list_t structure.
jhl_list_t* jhl_list_init();

// jhl_list_fin()
//
// Summary    : Destroys a jhl_list_t structure. Frees all allocated memory.
void jhl_list_fin_h(jhl_list_node_t* node);
void jhl_list_fin(jhl_list_t* list);
// Argument node is never NULL.
void jhl_list_fin_h(jhl_list_node_t* node);
// jhl_list_node_init()
//
// Summary    : Initializes a jhl_list_node.
// Note       : Use jhl_free() to destroy a node.
jhl_list_node_t* jhl_list_node_init(void* data);

// jhl_list_get_head()
void* jhl_list_get_head(jhl_list_t* list);

// jhl_list_get_tail()
void* jhl_list_get_tail(jhl_list_t* list);

// jhl_list_get_size()
int jhl_list_get_size(jhl_list_t* list);

// jhl_list_is_empty()
int jhl_list_is_empty(jhl_list_t* list);

// jhl_list_add_to_head()
//
// Summary    : Adds new data to the head of jhl_list_t structure.
void jhl_list_add_to_head(jhl_list_t* list, void* data);

// jhl_list_add_to_tail()
//
// Summary    : Adds new data to the tail of jhl_list_t structure.
void jhl_list_add_to_tail(jhl_list_t* list, void* data);

// jhl_list_contains()
//
// Summary   : Checks if the jhl_list_ structure contains data pointed by the argument pointer.
// Arguments : list -- jhl_list_t structure
//             data -- pointer to the data being checked
// Returns   : 1 if data is found.
//             0 otherwise.
int jhl_list_contains_h(jhl_list_node_t* node, void* data);
int jhl_list_contains(jhl_list_t* list, void* data);
// Argument node is never NULL.
int jhl_list_contains_h(jhl_list_node_t* node, void* data);

// jhl_list_remove_head()
//
// Summary    : Removes the first item of the jhl_list_t structure.
int jhl_list_remove_head(jhl_list_t* list);

// jhl_list_remove_tail()
//
// Summary    : Removes the last item of the jhl_list_t structure.
// Note       : This will take theta(n) time, where n is the size of the list.
int jhl_list_remove_tail(jhl_list_t* list);

////////////////////////////////////////////////////////////////////////////////////////////////////
// jhl_stack_t structure                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_stack_t structure
//
// Summary    : Implementation of a simple stack structure.
// Note       : Implementation is done with jhl_list_t.
typedef jhl_list_t jhl_stack_t;

// jhl_stack_init()
//
// Summary    : Initializes a jhl_stack_t structure.
jhl_stack_t* jhl_stack_init();

// jhl_stack_fin()
//
// Summary   : Destroys a jhl_stack_t structure.
void jhl_stack_fin(jhl_stack_t* stack);

// jhl_stack_push()
//
// Summary   : Stores an item into a jhl_stack_t structure.
void jhl_stack_push(jhl_stack_t* stack, void* data);
// jhl_stack_pop()
//
// Summary   : Retrieves an item from a jhl_stack_t structure.
//             Also removes it from the structure.
void* jhl_stack_pop(jhl_stack_t* stack);

////////////////////////////////////////////////////////////////////////////////////////////////////
// jhl_queue_t structure                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_queue_t structure
//
// Summary    : Implementation of a simple queue structure.
// Note       : Implementation is done with jhl_list_t.
typedef jhl_list_t jhl_queue_t;

// jhl_queue_init()
//
// Summary    : Initializes a jhl_queue_t structure.
jhl_queue_t* jhl_queue_init();

// jhl_queue_fin()
//
// Summary   : Destroys a jhl_queue_t structure.
void jhl_queue_fin(jhl_queue_t* queue);

// jhl_queue_push()
//
// Summary   : Stores an item into a jhl_queue_t structure.
void jhl_queue_enqueue(jhl_queue_t* queue, void* data);

// jhl_queue_pop()
//
// Summary   : Retrieves an item from a jhl_queue_t structure.
//             Also removes it from the structure.
void* jhl_queue_dequeue(jhl_queue_t* queue);

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
int jhl_string_brackets(char* result, char* open, char* close, char* string, int nested);

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
int jhl_string_shift(char* string, int length, int n);

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
int jhl_string_remove(char* string, char* remove, int n) ;

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
int jhl_string_split(char* result, char* remainder, char* marker, char* string, int repeat);

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
char* jhl_file_read(char* file_name, int* file_length);
char* jhl_file_read2(char* file_name, int* file_length);

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
int jhl_socket_prepare(int server_port, int connection_max);

// jhl_socket_accept()
//
// Summary   : Accepts an incoming socket connection request.
//             Performs accept().
// Arguments : listen_fd -- file descriptor for the listening socket
// Returns   : file descriptor for the connected socket if successful
//             -1 if accept() fails
// Note      : This is a blocking function.
int jhl_socket_accept(int listen_fd);
// jhl_socket_close()
//
// Summary   : Closes a socket connection.
//             Shorthand for close().
// Arguments : socket_fd -- file descriptor for the socket
// Returns   : 0  if successful
//             -1 if close() fails
int jhl_socket_close(int socket_fd);

// jhl_socket_connect()
//
// Summary   : Connects to a listening socket.
//             Performs socket() and connect().
// Arguments : server_IP_address -- the server's IP address
//             server_port       -- the server's port
// Returns   : file descriptor for the connected socket if successful
//             -1 if socket() fails
//             -2 if connect() fails
int jhl_socket_connect(char* server_IP_address, int server_port);

// jhl_socket_send()
//
// Summary   : Shorthand for send().
// Arguments : socket_fd   -- file descriptor for socket
//             buffer      -- pointer to the buffer to send data
//             buffer_size -- size of buffer
// Returns   : number of bytes of data sent if successful
//             -1 if send() fails
int jhl_socket_send(int socket_fd, char* buffer, size_t buffer_size);

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
int jhl_socket_receive(int socket_fd, char* buffer, size_t buffer_size);

////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory methods                                                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_malloc_record_t structure for maintaining a list of pointers to which memory is allocated.
typedef struct jhl_malloc_record_ jhl_malloc_record_t;
struct jhl_malloc_record_ {
	void* ptr;
	jhl_malloc_record_t* next;
};
// jhl_malloc_record_t* jhl_malloc_record = NULL;

// jhl_malloc_record_add()
//
// Summary    : Helper method for jhl_malloc().
//              Adds a node to the list of memory-allocated pointers after calling malloc().
void jhl_malloc_record_add(void* ptr);

// jhl_malloc_record_remove()
//
// Summary    : Helper method for jhl_free().
//              Removes a node from the list of memory-allocated pointers before calling free().
int jhl_malloc_record_remove(void* ptr);

// jhl_malloc()
//
// Summary   : Shorthand for memory allocation (malloc()).
//             Performs error check and memset().
// Arguments : size -- amount of memory allocated
// Caveat    : This will exit the program is memory allocation error occurs.
//             The assumption is that the program will run into a fatal error anyway
//             if allocation fails. If failure is addressed, then malloc() should be used.
// Returns   : Pointer to the allocated memory.
void* jhl_malloc(size_t size);

// jhl_free()
//
// Summary   :
// Arguments : ptr -- pointer to the memory allocated
void jhl_free(void* ptr);

// jhl_free_all()
//
// Summary   : Frees all memory allocated by jhl_malloc().
void jhl_free_all();

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timer methods                                                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////

// jhl_timer_start()
//
// Summary   : Starts the timer for jhl_time variable.
// Note      : jhl_time is unsigned long int in nanoseconds.
// Usage     : Once timer is started, retrieve the time by calling jhl_timer_get_time().
#define JHL_TIME_INTERVAL 0.1 // in seconds
// unsigned long int jhl_time;
// pthread_t jhl_time_thread;
// pthread_mutex_t jhl_time_lock = PTHREAD_MUTEX_INITIALIZER;
void* jhl_timer_start_h();
void jhl_timer_start();
void* jhl_timer_start_h();

// jhl_timer_get_time()
//
// Summary   : Gets the value of jhl_time variable.
unsigned long int jhl_timer_get_time();

// jhl_timer_get_time()
//
// Summary   : Gets the value of jhl_time variable.
void jhl_timer_set_time(unsigned long int time);

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
#define JHL_LOG_OFF 0
#define JHL_LOG_ON  1
int JHL_LOG;
/*int JHL_LOG       = JHL_LOG_ON;
int jhl_log_line  = 0;
int jhl_log_errno = 0;
pthread_mutex_t jhl_log_lock = PTHREAD_MUTEX_INITIALIZER;
*/
void jhl_log(char* message);

// jhl_log_on()
//
// Summary    : Enables jhl_log().
// Note       : See jhl_log().
void jhl_log_on();

// jhl_log_off()
//
// Summary    : Disables jhl_log().
// Note       : See jhl_log().
void jhl_log_off();
// jhl_log_here()
//
// Summary   : Prints a message that stands out. This is for quick debugging purposes, for finding
//             out whether the program reaches a particular line of code or not.
// Note      : Perhaps a good idea is to define a macro with a shorter name.
//             e.g., #define here() jhl_log_here()
#define JHL_LOG_HERE_MESSAGE "\n+--------------+\n|jhl_log_here()|\n+--------------+\n\n"
void jhl_log_here();

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
void jhl_clean(void* ptr, size_t size);

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
void jhl_check(int value, char* message);

// jhl_sleep()
//
// Summary   : Shorthand for nanosleep().
// Arguments : time -- amount of time for sleeping in seconds, in double
//                     i.e., a positive rational number.
int jhl_sleep(double time);

// jhl_deep_sleep()
//
// Summary   : Infinite loop.
void jhl_deep_sleep();

#endif
