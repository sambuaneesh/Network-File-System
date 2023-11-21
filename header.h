#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_REDUNDANT_SERVERS 3
#define MAX_FILE_SIZE 10000
#define MAX_NUM_PATHS 2000
#define MAX_FILE_PATH 500
#define MAX_FILE_NAME 100
#define MAX_NUM_FILES 10
#define COMMAND_SIZE 10
#define CACHE_SIZE 10
#define PORT 5566// port number for naming server
#define IP_NM "127.0.0.1"
#define CHECK_HEALTH_INTERVAL 5

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define PINK "\033[35m"
#define CYAN "\033[36m"
#define RESET "\033[0m"

//Error codes
#define INVALID_PATH "ERROR 101: Path not in list of accessible paths!"
#define COPY_TO_FILE "ERROR 102: Cannot copy to file!"
#define DIR_WRITE "ERROR 103: Cannot write to a directory!"
#define DIR_READ "ERROR 104: Cannot read from a directory!"
#define DIR_PERM "ERROR 105: Cannot get permissions of a directory!"
#define FILE_EXISTS "ERROR 106: Path Already Exists!"
#define DIR_EXISTS "ERROR 106: Path Already Exists!"
#define FILE_DEL "ERROR 107: Error Removing File!"
#define DIR_DEL "ERROR 108: Error Removing Directory!"
#define FILE_OPT "ERROR 109: Option Chosen to Copy a File, not a Directory!"
#define DIR_OPT "ERROR 110: Option Chosen to Copy a Directory, not a File!"
#define WRONG_DEL_DIR "ERROR 111: Cannot delete a file using delete directory!"
#define WRONG_DEL_FILE "ERROR 112: Cannot delete a directory using delete file!"
#define REC_USE "ERROR 113: Resource is being used by another client!"

typedef struct TreeNode* Tree;
typedef struct TreeNode {
    char path[MAX_FILE_PATH];// changed to path, cause first node will have the path of the ss
    Tree first_child;
    Tree next_sibling;
    Tree prev_sibling;
    Tree parent;
} TreeNode;

struct path_details {
    char path[MAX_FILE_PATH];
    int is_dir;
    char contents[MAX_FILE_SIZE];
    struct path_details* next;
};

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

typedef struct ss_send {
    char ip_addr[20];
    int client_port;
    int server_port;
    char ss_directory[MAX_FILE_PATH];
} ss_send;

typedef struct ss* storage_servers;
typedef struct ss {
    ss_send* ss_send;
    Tree files_and_dirs;
    storage_servers next;
} ss;

// Logic for caching
// 1. If the command is copy, then we need to check if the source path is in the cache
// 2. If the source path is in the cache, then we need to check if the destination path is in the cache
// 3. If the destination path is in the cache, then we need to check if the source path is the same as the one in the cache
// 4. If the source path is the same as the one in the cache, then we need to check if the destination path is the same as the one in the cache
// 5. If the destination path is the same as the one in the cache, then we need to check if the source path is the same as the one in the cache
// 6. If the source path is the same as the one in the cache, then we need to check if the destination path is the same as the one in the cache
// 7. If the destination path is the same as the one in the cache, then we need to check if the source path is the same as the one in the cache

typedef struct CacheStore {
    char command[COMMAND_SIZE];
    char source_path[MAX_FILE_PATH];
    char dest_path[MAX_FILE_PATH];
    storage_servers ss;
} CacheStore;

typedef struct Cache {
    int num_cache_entries;
    CacheStore cache_store[CACHE_SIZE];
} Cache;

struct client_thread_args {
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size;
    Cache cache;
    int nm_sock;
    struct sockaddr_in server_addr;
    int ss_sock;
    struct sockaddr_in ss_addr;
    socklen_t ss_addr_size;
    int ns_sock;
    struct sockaddr_in ns_addr;
};

struct ss_thread_args {
    char command[2];
    int received;
    char file_path[MAX_FILE_PATH];
    int naming_server_sock;
    int sock_ss_client;
    int client_sock;
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_size;
};


// Define a structure to store file name and its unique number
typedef struct {
    char* name;
    unsigned int uniqueNumber;
} FileMapping;

extern storage_servers storage_server_list;
extern int num_ss;
extern FileMapping fileMappings[MAX_NUM_FILES];// Global array to store mappings
extern unsigned int counter;                   // Global counter for unique numbers
extern int redundantCounter;
extern storage_servers redundantServers[3];


Tree Insert(Tree parent, char* path);
Tree MakeNode(char* name);
Tree Search_Till_Parent(Tree T, char* path, int insert);// returns the parent of the
    // node if path is found or inserts one new node if only one new node is asked
    // to be made or returns NULL if many new nodes need to be made
void PrintTree(Tree T);
int Delete_Path(Tree T, char* path, char* ss_dir);
void Del_Rec(Tree T);
void PrintAll();

void load_SS(Tree T, char* file_name, char* ss_dir);
void close_socket(int* client_sock);
void connect_to_naming_server(char* ip, int* sock, struct sockaddr_in* addr);
void listen_for_client(int* server_sock,
                       int* client_sock,
                       struct sockaddr_in* client_addr,
                       socklen_t* addr_size);
void open_naming_server_port(int port_number, int* server_sock, struct sockaddr_in* server_addr);
void connect_to_SS_from_NS(int* ns_sock, struct sockaddr_in* ns_addr, int port_num);
storage_servers MakeNode_ss(char* ip_addr, int client_port, int server_port, char* init_path);
storage_servers check_if_path_in_ss(char* file_path, int insert);
int initialize_SS(int* ss_sock);

void get_path_details(char* path_to_go_to, char* file_name, char* file_path);
char* get_partial_path(char* path1, char* path2);


int create_file(char* file_path);
int create_directory(char* file_path);
int delete_file(char* file_path);
int delete_directory(char* file_path);
int delete_non_empty_dir(char* directory_name);
int copy_file(char* source_path, char* dest_path, char* buffer);
int copy_directory(char* source_path, char* dest_path, char* buffer, char* path_file, char* dest);

int Delete_from_path_file(char* file_path, char* storage_file);
int Add_to_path_file(char* file_path, char* storage_file);

void connect_to_SS_from_client(int* sock, struct sockaddr_in* addr, char* ns_ip, int ns_port);

void make_socket_non_blocking(int socket);
void MakeSSsend_vital(
    int* naming_server_sock, char* ip, int* port_for_client, int* port_for_nm, char* paths_file);
void init_port_create_sock(int* sock, struct sockaddr_in* addr, const char* ip, int port_num);

int copy_file_for_dir(char* source_path, char* dest_path);
void get_full_path(char* path, char* buffer);
int isSuffix(const char* mainString, const char* suffix);
storage_servers find_ss(char* file_path);


Cache InitCache();
storage_servers CheckCache(Cache cache, char* command, char* source_path, char* dest_path);
void InsertIntoCache(
    Cache cache, char* command, char* source_path, char* dest_path, storage_servers ss);

int isPortAvailable(int p);
void* health_thread(void* arg);
void delete_ss(char* ip_addr, int port);
int checkSS(int* ns_sock, struct sockaddr_in* ns_addr, int port_num);
int mapToRange(const char* name);
struct path_details* readPathfile(const char* ip_addr, int port);
void copy_files_to_SS(struct path_details* pathsfile, const char* ip_addr, int port);
int isRedundantServer(char* ip, int port);