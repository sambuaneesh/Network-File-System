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
        {
            printf("\nDo you want to:\n");
            printf("1. Connect to a storage server     (enter 1)\n");
            printf("2. Delete a File/Directory         (enter 2)\n");
            printf("3. Create an Empty File/Directory  (enter 3)\n");
            printf("4. Copy Files/Directories          (enter 4)\n");
            printf("5. Edit File                       (enter 5)\n");
            printf("6. Read File                       (enter 6)\n");
            printf("7. Write To File                   (enter 7)\n");
            printf("8. Get File Info                   (enter 8)\n");
        }

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
        else if (option == 3) // Creation
        {
            char option_client[2];
            strcpy(option_client, "3");

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
        else
        {
            printf("Invalid option\n");
        }
    }

    // Close the client socket when done
    close(client_sock);

    return 0;
}