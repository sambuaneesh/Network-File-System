CC = gcc
# CFLAGS = -g -pthread -Wall
CFLAGS = -g -pthread
SRC = client.c storageserver.c namingserver.c functions.c
OBJ = $(SRC:.c=.o)
TARGETS = client ss nm

all: $(TARGETS)

client: client.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

ss: storageserver.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

nm: namingserver.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.c header.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGETS)

re: clean all

test: ss
	cp ss ss1
	cp ss ss2
	cp ss ss3
	cp ss ss4

rem : 
	rm ss1/ss
	rm ss2/ss
	rm ss3/ss
	rm ss4/ss