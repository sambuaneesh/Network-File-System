#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_PATH_SIZE 200
#define port 5566               // port number for naming server

typedef struct paths
{
    char path[MAX_PATH_SIZE];
    struct paths *next;
} paths;
// basically a linked list of paths

// for now, I am assuming that the storage server sends all paths in the format:
//
// ├── A
// ├── B
// │   ├── B1
// │   └── B2
// ├── C
// └── D
//     ├── D1
//     └── D2
//
// as:
// A -> B -> B/B1 -> B/B2 -> C -> D -> D/D1 -> D/D2

typedef struct ss_send
{
    char ip_addr[20];
    int client_port;
    int server_port;
    paths *files_and_dirs;
} ss_send;

typedef struct ss* storage_servers;
typedef struct ss
{
    ss_send *ss_send;
    storage_servers next;
    int num_server;
} ss;

void close_socket(int* client_sock);
void connect_to_naming_server(char *ip, int *sock, struct sockaddr_in *addr);
void listen_for_client(int* server_sock, int* client_sock, struct sockaddr_in *client_addr, socklen_t *addr_size);
void open_naming_server_port(int port_number, int *server_sock, struct sockaddr_in *server_addr);