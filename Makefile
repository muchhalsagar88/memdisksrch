#
#
CC=gcc
CFLAGS=-g

# comment line below for Linux machines
#LIB= -lsocket -lnsl

all: assn_1

assn_1:	assn_1.o
	$(CC) $(CFLAGS) -o $@ assn_1.o $(LIB)

assn_1.o:	structs.h
	$(CC) $(CFLAGS) -c assn_1.c

clean:
	\rm -f assn_1

squeaky:
	make clean
	\rm -f assn_1.o