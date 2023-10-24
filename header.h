#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>


#define MAX_FILE_PATH 500
#define MAX_FILE_NAME 100
#define MAX_NUM_FILES 10
#define port 5566               // port number for naming server

#define RED "\033[31m"
#define RESET "\033[0m"

typedef struct TreeNode* Tree;
typedef struct TreeNode
{
    char path[MAX_FILE_NAME];
    Tree first_child;
    Tree next_sibling;
    Tree prev_sibling;
    Tree parent;
} TreeNode;

typedef struct paths
{
    char path[MAX_FILE_PATH];
    struct paths *next;
} paths;

// for now, I am assuming that the storage server sends all paths in the format:
//
// Trees somehow
// ├── A
// ├── B
// │   ├── B1
// │   └── B2
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
} ss_send;

typedef struct ss* storage_servers;
typedef struct ss
{
    ss_send *ss_send;
    Tree files_and_dirs;
    storage_servers next;
} ss;

extern storage_servers storage_server_list;

Tree Insert(Tree parent, char *path);
Tree MakeNode(char *name);
Tree Search_Till_Parent(Tree T, char *path, int insert); // returns the parent of the 
        // node if path is found or inserts one new node if only one new node is asked
        // to be made or returns NULL if many new nodes need to be made
void PrintTree(Tree T);
void Delete_Path(Tree T, char *path);
void Del_Rec(Tree T);

void load_SS(Tree T,char* file_name);
void close_socket(int* client_sock);
void connect_to_naming_server(char *ip, int *sock, struct sockaddr_in *addr);
void listen_for_client(int* server_sock, int* client_sock, struct sockaddr_in *client_addr, socklen_t *addr_size);
void open_naming_server_port(int port_number, int *server_sock, struct sockaddr_in *server_addr);
void connect_to_NS_from_SS(int* sock, struct sockaddr_in* addr, const char* ip, int port_num);
void connect_to_SS_from_NS(int* ns_sock, struct sockaddr_in* ns_addr,int port_num);
storage_servers MakeNode_ss(char *ip_addr, int client_port, int server_port);
int check_if_path_in_ss(char *file_path);

void get_path_details(char* path_to_go_to, char* file_name,char* file_path);
void create_file(char* file_path);
void create_directory(char* file_path);
void delete_file(char* file_path);
void delete_directory(char* file_path);
void delete_non_empty_dir(char *directory_name);

void Delete_from_path_file(char* file_path);
int Add_to_path_file(char *file_path);

void connect_to_SS_from_client(int* sock, struct sockaddr_in* addr, char* ns_ip, int ns_port);
void connect_to_client(int* sock, struct sockaddr_in* addr, const char* ip, int port_num);
