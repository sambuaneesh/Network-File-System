#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_FILE_PATH 500
#define MAX_FILE_NAME 100
#define MAX_NUM_FILES 10
#define port 5566               // port number for naming server

typedef struct TreeNode* Tree;
typedef struct TreeNode
{
    char path[MAX_FILE_NAME];
    Tree first_child;
    Tree next_sibling;
} TreeNode;

// for now, I am assuming that the storage server sends all paths in the format:
//
// Trees somehow
// ├── A
// ├── B
// │   ├── B1
// │   └── B2
// ├── C
// └── D
//     ├── D1
//     └── D2
//
// .
// |
// A -> B -> C -> D
//      |         |
//      B1->B2    D1->D2
// assume that everything in the dir is sent how to send 
// as:
// n-ary tree
// we always have a root node

typedef struct ss_send
{
    char ip_addr[20];
    int client_port;
    int server_port;
    Tree *files_and_dirs;
} ss_send;

typedef struct ss* storage_servers;
typedef struct ss
{
    ss_send *ss_send;
    storage_servers next;
    int num_server;
} ss;

Tree Insert(Tree parent, char *path);
Tree MakeNode(char *name);
Tree Search_Till_Parent(Tree T, char *path);
void PrintTree(Tree T);

void close_socket(int* client_sock);
void connect_to_naming_server(char *ip, int *sock, struct sockaddr_in *addr);
void listen_for_client(int* server_sock, int* client_sock, struct sockaddr_in *client_addr, socklen_t *addr_size);
void open_naming_server_port(int port_number, int *server_sock, struct sockaddr_in *server_addr);