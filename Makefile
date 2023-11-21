CC = gcc
CFLAGS = -g -pthread
SRC = client.c storageserver.c namingserver.c functions.c
OBJ = $(SRC:.c=.o)
TARGETS = client ss nm
DIRS = ss1 ss2 ss3

all: $(TARGETS) create_directories create_paths copy_ss

client: client.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

ss: storageserver.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

nm: namingserver.o functions.o
	$(CC) $^ -o $@ $(CFLAGS)

%.o: %.c header.h
	$(CC) -c $< -o $@ $(CFLAGS)

create_directories:
	mkdir -p $(DIRS)

.PHONY: create_paths
create_paths:
	@echo "Creating paths.txt in ss1..."
	@mkdir -p ss1
	@echo "/" > ss1/paths.txt

	@echo "Creating paths.txt in ss2..."
	@mkdir -p ss2
	@echo "/" > ss2/paths.txt

	@echo "Creating paths.txt in ss3..."
	@mkdir -p ss3
	@echo "/" > ss3/paths.txt

copy_ss: ss
	cp ss ss1
	cp ss ss2
	cp ss ss3

clean:
	rm -f $(OBJ) $(TARGETS)
	rm -rf $(DIRS)

re: clean all

# .PHONY: dev run_ss stop_ss

# dev: run_ss
# 	# Add any other development-related tasks here

# run_ss: $(DIRS) ss
# 	./ss1/ss &
# 	./ss2/ss &
# 	./ss3/ss &

# stop_ss:
# 	pkill -f "ss1/ss"
# 	pkill -f "ss2/ss"
# 	pkill -f "ss3/ss"