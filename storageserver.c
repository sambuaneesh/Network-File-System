#include "header.h"

int main()
{
    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    char *ip = "127.0.0.1"; // replace with function that gets current ip addr

    connect_to_naming_server(ip, &sock, &addr);

    // send data to naming server
    printf("Enter port for communication with naming server: \n");
    int server_port;
    scanf("%d", &server_port);
    printf("Enter port for communication with client: \n");
    int client_port;
    scanf("%d", &client_port);

    ss_send *data = (ss_send *)malloc(sizeof(ss_send));
    strcpy(data->ip_addr, ip);
    data->client_port = client_port;
    data->server_port = server_port;
    data->files_and_dirs = NULL;
    printf("Data sent to naming server.\n");

    // send data to naming server
    send(sock, data, sizeof(ss_send), 0);
    close(sock);
    connect_to_naming_server(ip, &sock, &addr);
    return 0;
}