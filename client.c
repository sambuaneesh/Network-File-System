#include "header.h"

int main()
{
    int naming_server_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_size;

    // Create a socket and connect to the naming server
    connect_to_naming_server("127.0.0.1", &naming_server_sock, &client_addr);

    while (1)
    {
        int option = -1;
        {
            printf(CYAN "\nDo you want to:\n");
            // printf("1. Connect to a storage server     (enter 1)\n");
            printf("2. Delete a File/Directory         (enter 2)\n");
            printf("3. Create an Empty File/Directory  (enter 3)\n");
            printf("4. Copy Files/Directories          (enter 4)\n");
            printf("5. Write To File                   (enter 5)\n");
            printf("6. Read File                       (enter 6)\n");
            printf("7. Get File Info                   (enter 7)\n" RESET);
        }

        scanf("%d", &option);

        if (option == 1)
        {
            if (send(naming_server_sock, "1", strlen("1"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
        }
        else if (option == 2) // Deletion
        {
            char option_client[2] = "2";
            option_client[strlen(option_client)] = '\0';

            // Send option to NS
            if (send(naming_server_sock, option_client, sizeof(option_client), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            char path[MAX_FILE_PATH];
            printf(CYAN "Enter the path: " RESET);
            scanf("%s", path);

            char delete_option[10];

            printf(CYAN "Do you want to\n");
            printf("Delete a file       (Enter 1)\n");
            printf("Delete a directory  (Enter 2)\n" RESET);
            scanf("%s", delete_option);

            // Send the path to NS
            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
            {
                perror(RED "[-] Send error\n" RESET);
                exit(1);
            }
            // Send whether you want to create a file or directory to NS
            if (send(naming_server_sock, delete_option, sizeof(delete_option), 0) == -1)
            {
                printf(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // receive success or error message from NS
            char success[25];
            if (recv(naming_server_sock, success, sizeof(success), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            if (strcmp(success, "done") == 0)
                printf(GREEN "Deleted Successfully!\n" RESET);
            else
                printf(RED "Error deleting file/directory\n" RESET);
        }
        else if (option == 3) // Creation
        {
            char option_client[2];
            strcpy(option_client, "3");

            // Send option to NS
            if (send(naming_server_sock, option_client, sizeof(option_client), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            char path[MAX_FILE_PATH];
            printf(CYAN "Enter the path: " RESET);
            scanf("%s", path);

            char create_option[10];

            printf(CYAN "Do you want to\n");
            printf("Create an empty file       (Enter 1)\n");
            printf("Create an empty directory  (Enter 2)\n" RESET);
            scanf("%s", create_option);

            // Send the path to NS
            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }
            // Send whether you want to create a file or directory to NS
            if (send(naming_server_sock, create_option, sizeof(create_option), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // recieve success or error message from NS
            char success[25];
            if (recv(naming_server_sock, success, sizeof(success), 0) == -1)
            {
                perror(RED "[-] Receive error\n" RESET);
                exit(1);
            }
            if (strcmp(success, "done") == 0)
                printf(GREEN "Created Successfully!\n" RESET);
            else
                printf(RED "Error creating file/directory\n" RESET);
        }
        else if (option == 4)
        {
        }
          else if (option == 5) // Write
        {
            if (send(naming_server_sock, "5", strlen("5"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");

            char ip_addr[50];
            char server_addr[50];

            if (recv(naming_server_sock, ip_addr, sizeof(ip_addr), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            if (strcmp(ip_addr, "failed") == 0)
            {
                printf(RED "File does not exist\n" RESET);
                continue;
            }
            if (recv(naming_server_sock, server_addr, sizeof(server_addr), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }

            int ns_sock;
            struct sockaddr_in ns_addr;
            connect_to_SS_from_client(&ns_sock, &ns_addr, ip_addr, atoi(server_addr));
            if (send(ns_sock, path, sizeof(path), 0) == -1)
            {
                printf("[-] Send error\n");
                close_socket(&ns_sock);
                return 1; // Return an error code
            }

            char input[1024];

            printf("Start entering data: (Enter 'done' to stop): \n");
            while (1)
            {

                scanf(" %[^\n]s", input);
                // strcat(input, "\n");
                // input[strlen(input)] = '\0';

                if (strcmp(input, "done") == 0)
                {
                    if (send(ns_sock, input, sizeof(input), 0) == -1)
                    {
                        printf("[-] Send error\n");
                        break;
                    }
                    break;
                }
                else
                {

                    if (send(ns_sock, input, sizeof(input), 0) == -1)
                    {
                        printf("[-] Send error\n");
                        break;
                    }
                }
            }

            close_socket(&ns_sock);
        }
        else if (option == 6) // Read
        {
            if (send(naming_server_sock, "6", strlen("6"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");

            char ip_addr[50];
            char server_addr[50];

            if (recv(naming_server_sock, ip_addr, sizeof(ip_addr), 0) == -1)
            {
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }
            if (strcmp(ip_addr, "failed") == 0)
            {
                printf(RED "File does not exist\n" RESET);
                continue;
            }
            if (recv(naming_server_sock, server_addr, sizeof(server_addr), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            int ns_sock;
            struct sockaddr_in ns_addr;
            connect_to_SS_from_client(&ns_sock, &ns_addr, ip_addr, atoi(server_addr));
            if (send(ns_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");

            // Getting file contents
            char buffer[1024];
            int c = 0;
            while (c == 0)
            {
                recv(ns_sock, buffer, sizeof(buffer), 0);
                if (strcmp("DONE", buffer) == 0)
                {
                    c = 2; // DONE WITH FILE
                    printf("Finished reading file!\n");
                }
                else
                {
                    printf(PINK "%s" RESET, buffer);
                }
            }

            close_socket(&ns_sock);
        }
        else if (option == 7) // Permissions
        {
            if (send(naming_server_sock, "7", strlen("7"), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");

            char ip_addr[50];
            char server_addr[50];

            if (recv(naming_server_sock, ip_addr, sizeof(ip_addr), 0) == -1)
            {
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }
            if (strcmp(ip_addr, "failed") == 0)
            {
                printf(RED "File does not exist\n" RESET);
                continue;
            }
            if (recv(naming_server_sock, server_addr, sizeof(server_addr), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            int ns_sock;
            struct sockaddr_in ns_addr;
            connect_to_SS_from_client(&ns_sock, &ns_addr, ip_addr, atoi(server_addr));
            if (send(ns_sock, path, sizeof(path), 0) == -1)
                printf(RED "[-]Send error\n" RESET);
            char permission[1024];

            if (recv(ns_sock, permission, sizeof(permission), 0) == -1)
            {
                perror(RED "Error receiving data" RESET);
                exit(0);
            }
            printf("\n");
            printf(YELLOW "%s" RESET, permission);

            close_socket(&ns_sock);
        }
        else
        {
            printf(RED "Invalid option\n" RESET);
        }
    }

    // Close the client socket when done
    close(naming_server_sock);

    return 0;
}