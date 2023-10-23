#include "header.h"

int main()
{

    int sock, client_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    char *ip = "127.0.0.1";

    // Connect to SS
    connect_to_NS_from_SS(&sock, &addr, "127.0.0.1", 5566);

    // Recieving the command number from the naming server
    // while (1)
    // {

    // char file_path[1000];
    // char command[1000];
    // int received;
    //     if ((received = recv(sock, &command, sizeof(command), 0)) == -1)
    //     {
    //         printf("Error recieving data\n");
    //         exit(0);
    //     }
    //     else
    //     {
    //         command[received] = '\0';
    //     }

    //     if (strcmp(command, "connect") == 0)
    //     {

    //         // send data to naming server
    //         printf("Enter port for communication with naming server: \n");
    //         int server_port;
    //         scanf("%d", &server_port);
    //         printf("Enter port for communication with client: \n");
    //         int client_port;
    //         scanf("%d", &client_port);

    //         ss_send *data = (ss_send *)malloc(sizeof(ss_send));
    //         strcpy(data->ip_addr, ip);
    //         data->client_port = client_port;
    //         data->server_port = server_port;
    //         data->files_and_dirs = NULL;
    //         printf("Data sent to naming server.\n");

    //         // send data to naming server
    //         int s = send(sock, data, sizeof(ss_send), 0);

    //         close(sock);
    //         connect_to_naming_server(ip, &sock, &addr);
    //     }

    //     else if (strcmp(command, "create") == 0)
    //     {

    //         // Getting the file path
    //    if ((received = recv(sock, &file_path, sizeof(file_path), 0)) == -1)
    // {
    //     printf("Error recieving data\n");
    //     exit(0);
    // }
    // else
    // {
    //     file_path[received] = '\0';
    // }
    // char* temp = (char*)malloc(sizeof(char)*100);
    // strcpy(temp,file_path);
    //  char option[10];
    //   if ((received = recv(sock, &option, sizeof(option), 0)) == -1)
    // {
    //     printf("Error recieving data\n");
    //     exit(0);
    // }
    // else
    // {
    //     option[received] = '\0';
    // }

    // // If option is 1, create a file, if option is 2, create a directory
    // if(strcmp(option,"1")==0){
    //     create_file(temp);
    // }
    // else if(strcmp(option,"2")==0){
    //     create_directory(temp);
    // }
    // // Sending success message
    // int sent = send(sock, "done",sizeof("done"), 0);
    //     }

    //     else if (strcmp(command, "delete") == 0)
    //     {

    //         // Getting the file path
    //        if ((received = recv(sock, &file_path, sizeof(file_path), 0)) == -1)
    //     {
    //         printf("Error recieving data\n");
    //         exit(0);
    //     }
    //     else
    //     {
    //         file_path[received] = '\0';
    //     }

    //     char* temp = (char*)malloc(sizeof(char)*100);
    //     strcpy(temp,file_path);

    //  char option[10];
    //   if ((received = recv(sock, &option, sizeof(option), 0)) == -1)
    // {

    //     printf("Error recieving data\n");
    //     exit(0);
    // }
    // else
    // {
    //     option[received] = '\0';
    // }

    //     // If option is 1, delete a file, if option is 2, delete a directory
    //     if(strcmp(option,"1")==0){
    //         delete_file(temp);
    //     }
    //     else if(strcmp(option,"2")==0){
    //         delete_directory(temp);
    //     }
    //     // Sending success message
    //     int sent = send(sock, "done",sizeof("done"), 0);
    //     }
    // }

    addr_size = sizeof(client_addr);
    //    Accept incoming connections
    client_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_size);
    if (client_sock == -1)
    {
        perror("[-] Accept error");
        exit(0);
    }
    
    while (1)
    {

        // Defining variables for sending and recieving
        char file_path[MAX_FILE_PATH];
        char command[2];
        int received;

        if ((received = recv(client_sock, &command, sizeof(command), 0)) == -1)
        {
            printf("Error recieving data\n");
            exit(0);
        }
        else
        {
            command[received] = '\0';
        }

        if (strcmp(command, "1") == 0)
        {
        }
        else if (strcmp(command, "2") == 0)
        {
        }
        else if (strcmp(command, "3") == 0)
        {
            if ((received = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                file_path[received] = '\0';
            }

            char *temp = (char *)malloc(sizeof(char) * 1000);
            strcpy(temp, file_path);
            char option[10];

            if ((received = recv(client_sock, &option, sizeof(option), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                option[received] = '\0';
            }

            // If option is 1, create a file, if option is 2, create a directory
            if (strcmp(option, "1") == 0)
            {
                create_file(temp);
            }
            else if (strcmp(option, "2") == 0)
            {
                create_directory(temp);
            }
            // Sending success message
            int sent = send(client_sock, "done", sizeof("done"), 0);
            if (sent == -1)
            {
                perror("Error sending data");
            }
        }
    }
    close(client_sock);
    return 0;
}