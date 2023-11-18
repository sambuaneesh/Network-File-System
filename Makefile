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