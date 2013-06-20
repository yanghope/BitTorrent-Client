CC=gcc
CFLAGS=-Wall -pedantic -std=c99 -g -D_GNU_SOURCE
LIBS=-lpthread -lcrypto
TARGET=darttorrent

SOURCES=src/util/bencode.c \
	src/util/jhl_lib.c \
	src/util/queue.c \
	src/util/sha1.c \
	src/util/util.c \
	src/tracker/make_tracker_request.c \
	src/tracker/parse_announce_url.c \
        src/tracker/parsetorrentfile.c \
	src/tracker/process_tracker_response.c \
	src/pwp/pwp_bitfield.c \
	src/pwp/pwp_connection_listener.c \
	src/pwp/pwp_connection_manager.c \
	src/pwp/pwp_data_msgs.c \
	src/pwp/pwp_status_msgs.c \
	src/main/darttorrent.c \
	src/main/shutdown.c 

OBJS=   src/util/bencode.o \
	src/util/jhl_lib.o \
	src/util/queue.o \
	src/util/sha1.o \
	src/util/util.o \
	src/tracker/make_tracker_request.o \
	src/tracker/parse_announce_url.o \
        src/tracker/parsetorrentfile.o \
	src/tracker/process_tracker_response.o \
	src/pwp/pwp_bitfield.o \
	src/pwp/pwp_connection_listener.o \
	src/pwp/pwp_connection_manager.o \
	src/pwp/pwp_data_msgs.o \
	src/pwp/pwp_status_msgs.o \
	src/main/darttorrent.o \
	src/main/shutdown.o

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} -o bin/${TARGET} ${LIBS} ${OBJS}

%.o: $.c
	$(CC) -c $(CFLAGS) $@ $<

clean:
	rm -rf src/*/*.o
	rm -rf bin/${TARGET}
	rm -rf *.o
	rm -rf ${TARGET}
	rm -f core.*
	rm -f *~
	rm -f *#
	rm -f src/*/*~
	rm -f src/*/*#
	rm -f *.txt
	rm -f *.mpg

.PHONY: all clean
