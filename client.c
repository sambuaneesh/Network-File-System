#include "header.h"

int main()
{
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_size;

    // Create a socket and connect to the naming server
    connect_to_naming_server("127.0.0.1", &client_sock, &client_addr);

    while (1)
    {
        int option = -1;
        printf("\nDo you want to:\n");
        printf("Connect to a storage server     (enter 1)\n");
        printf("Delete a File/Directory         (enter 2)\n");
        printf("Create an Empty File/Directory  (enter 3)\n");
        printf("Copy Files/Directories          (enter 4)\n");
        printf("Edit File                       (enter 5)\n");
        printf("Read File                       (enter 6)\n");
        printf("Write To File                   (enter 7)\n");
        printf("Get File Info                   (enter 8)\n");

        scanf("%d", &option);

        if (option == 1)
        {
            if (send(client_sock, "1", strlen("1"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
        }
        else if (option == 2)
        {
            char option_client[2] = "2";
            option_client[strlen(option_client)] = '\0';

            // Send option to NS
            if (send(client_sock, option_client, sizeof(option_client), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }

            char storage_server_num[10];
            int num_storage_servers = 10;
            printf("Enter the storage server number: ");
            scanf("%s", storage_server_num);

            if (atoi(storage_server_num) > num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            char delete_option[10];

            printf("Do you want to\n");
            printf("Delete a file       (Enter 1)\n");
            printf("Delete a directory  (Enter 2)\n");
            scanf("%s", delete_option);
            // Send storage sever number to the NS
            if (send(client_sock, storage_server_num, sizeof(storage_server_num), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            // Send the path to NS
            if (send(client_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");
            // Send whether you want to create a file or directory to NS
            if (send(client_sock, delete_option, sizeof(delete_option), 0) == -1)
                printf("[-]Send error\n");
        }
        else if (option == 3)
        {

            char option_client[2] = "3";
            option_client[strlen(option_client)] = '\0';

            // Send option to NS
            if (send(client_sock, option_client, sizeof(option_client), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }

            char storage_server_num[10];
            int num_storage_servers = 10;
            printf("Enter the storage server number: ");
            scanf("%s", storage_server_num);

            if (atoi(storage_server_num) > num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            char create_option[10];

            printf("Do you want to\n");
            printf("Create an empty file       (Enter 1)\n");
            printf("Create an empty directory  (Enter 2)\n");
            scanf("%s", create_option);
            // Send storage sever number to the NS
            if (send(client_sock, storage_server_num, sizeof(storage_server_num), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            // Send the path to NS
            if (send(client_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");
            // Send whether you want to create a file or directory to NS
            if (send(client_sock, create_option, sizeof(create_option), 0) == -1)
                printf("[-]Send error\n");
        }
        else if (option == 4)
        {
        }
        else if (option == 5)
        {
        }
        else if (option == 6)
        {
            if (send(client_sock, "6", strlen("6"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            if (send(client_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");

            char ip_addr[50];
            char server_addr[50];

           if(recv(client_sock, ip_addr, sizeof(ip_addr), 0) == -1){
            perror("[-]Send error");
            exit(1);
           }
           if(recv(client_sock, server_addr, sizeof(server_addr), 0) == -1){
            perror("[-]Send error");
            exit(1);
           }
               int ns_sock;
    struct sockaddr_in ns_addr;
           connect_to_SS_from_NS(&ns_sock, &ns_addr,5568);
            if (send(ns_sock, "65", sizeof("65"), 0) == -1)
                printf("[-]Send error\n");
         close_socket(&ns_sock);
          
        }
        else
        {
            printf("Invalid option\n");
        }
    }

    // Close the client socket when done
    close(client_sock);

    return 0;
}
