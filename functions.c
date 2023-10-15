#include "header.h"

void close_socket(int* client_sock)
{
    if (close(*client_sock) == -1)
    {
        perror("[-]Close error");
        exit(1);
    }
    else
        printf("[+]Client disconnected.\n\n");
}

void connect_to_naming_server(char *ip, int *sock, struct sockaddr_in *addr)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(addr, '\0', sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = port;
    addr->sin_addr.s_addr = inet_addr(ip); // converts the string to an acceptable form

    if (connect(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
        printf("[-]Connect error");
    printf("Connected to the naming server.\n");
}

void listen_for_client(int *server_sock, int *client_sock, struct sockaddr_in *client_addr, socklen_t *addr_size)
{
    if (listen(*server_sock, 5) == -1)
    {
        perror("[-]Listen error");
        exit(1);
    }

    printf("Listening...\n");
    *addr_size = sizeof(*client_addr);
    *client_sock = accept(*server_sock, (struct sockaddr *)client_addr, addr_size);
    printf("[+]Storage Server connected.\n");
}

void open_naming_server_port(int port_number, int *server_sock, struct sockaddr_in *server_addr)
{
    char *ip = "127.0.0.1";
    int n;

    *server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(server_addr, '\0', sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = port;
    server_addr->sin_addr.s_addr = inet_addr(ip);

    n = bind(*server_sock, (struct sockaddr *)server_addr, sizeof(*server_addr));
    if (n < 0)
    {
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
}