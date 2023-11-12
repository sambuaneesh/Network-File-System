#include "header.h"

int main()
{
    int naming_server_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_size;

    // Create a socket and connect to the naming server
    connect_to_naming_server("127.0.0.1", &naming_server_sock, &client_addr);
    int role = 2;
    if (send(naming_server_sock, &role, sizeof(role), 0) == -1)
    {
        perror(RED "[-]Send error" RESET);
        exit(1);
    }

    while (1)
    {
        int option = -1;
        {
            printf(CYAN "\nDo you want to:\n");
            printf("1. Exit from program               (enter 1)\n");
            printf("2. Delete a File/Directory         (enter 2)\n");
            printf("3. Create an Empty File/Directory  (enter 3)\n");
            printf("4. Copy Files/Directories          (enter 4)\n");
            printf("5. Write To File                   (enter 5)\n");
            printf("6. Read File                       (enter 6)\n");
            printf("7. Get File Info                   (enter 7)\n" RESET);
        }

        scanf("%d", &option);

        if (option == 1) // Exiting from the program
        {
            if (send(naming_server_sock, "1", strlen("1"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            exit(0);
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

            char mid_ack[10];
            int ind = 0;
            if ((ind = recv(naming_server_sock, mid_ack, sizeof(mid_ack), 0)) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            mid_ack[ind] = '\0';
            // printf("IND: %d\n",ind);
            // printf("MID: %s\n",mid_ack);
            if (strcmp(mid_ack, "failed") == 0)
            {
                printf(RED "Invalid Path\n" RESET);
                continue;
            }

            // receive success or error message from NS
            char success[10];
            if (recv(naming_server_sock, success, sizeof(success), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Deleted Successfully!\n" RESET);
            }
            else
            {
                printf(RED "Error deleting file/directory\n" RESET);
            }
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

            char mid_ack[10];
            int ind = 0;
            if ((ind = recv(naming_server_sock, mid_ack, sizeof(mid_ack), 0)) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            //  mid_ack[ind]='\0';

            // printf("IND: %d\n",ind);
            // printf("MID: %s\n",mid_ack);
            if (strcmp(mid_ack, "failed") == 0)
            {
                printf(RED "Invalid Path\n" RESET);
                continue;
            }

            // recieve success or error message from NS
            char success[10];
            if ((ind = recv(naming_server_sock, success, sizeof(success), 0)) == -1)
            {
                perror(RED "[-] Receive error\n" RESET);
                exit(1);
            }
            //  success[ind]='\0';
            //    printf("SUCCESS: %s\n",success);
            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Created Successfully!\n" RESET);
            }
            else
            {
                printf(RED "Error creating file/directory\n" RESET);
            }
        }
        else if (option == 4) // Copying paths
        {

            char option_client[2] = "4";
            option_client[strlen(option_client)] = '\0';

            // Send option to NS
            if (send(naming_server_sock, option_client, sizeof(option_client), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            char source_path[MAX_FILE_PATH];
            printf(CYAN "Enter the source path: " RESET);
            scanf("%s", source_path);

            char dest_path[MAX_FILE_PATH];
            printf(CYAN "Enter the destination path: " RESET);
            scanf("%s", dest_path);

            char copy_option[10];

            printf(CYAN "Do you want to\n");
            printf("Copy a file       (Enter 1)\n");
            printf("Copy a directory  (Enter 2)\n" RESET);
            scanf("%s", copy_option);

            // Send the source path to NS
            if (send(naming_server_sock, source_path, sizeof(source_path), 0) == -1)
            {
                perror(RED "[-] Send error\n" RESET);
                exit(1);
            }
            // Send the  destination path to NS
            if (send(naming_server_sock, dest_path, sizeof(dest_path), 0) == -1)
            {
                perror(RED "[-] Send error\n" RESET);
                exit(1);
            }
            // Send whether you want to create a file or directory to NS
            if (send(naming_server_sock, copy_option, sizeof(copy_option), 0) == -1)
            {
                printf(RED "[-]Send error\n" RESET);
                exit(1);
            }

            char mid_ack[10];
            if (recv(naming_server_sock, mid_ack, sizeof(mid_ack), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            if (strcmp(mid_ack, "failed") == 0)
            {
                printf(RED "Invalid Path\n" RESET);
                continue;
            }

            // recieve success or error message from NS
            char success[10];
            if (recv(naming_server_sock, success, sizeof(success), 0) == -1)
            {
                perror(RED "[-] Receive error\n" RESET);
                exit(1);
            }
            if (strcmp(success, "done") == 0)
                printf(GREEN "Copied Successfully!\n" RESET);
            else
                printf(RED "Error copying file/directory\n" RESET);
        }
        else if (option == 5) // Write
        {
            if (send(naming_server_sock, "5", strlen("5"), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf(CYAN "Enter the path: " RESET);
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
            {
                perror("[-]Send error");
                exit(1);
            }

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
            char mid_ack[100];
            if (recv(ns_sock, mid_ack, sizeof(mid_ack), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            if (strcmp(mid_ack, "success") != 0)
            {
                printf(RED "%s\n" RESET, mid_ack);
                continue;
            }
            char input[1024];

            printf(CYAN "Start entering data: (Enter 'done' to stop): \n\n" RESET);
            while (1)
            {
                scanf(" %[^\n]s", input);

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
            printf(CYAN "Enter the path: " RESET);
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

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
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }
            strcpy(ip_addr, "127.0.0.1"); // Fix

            int ss_sock;
            struct sockaddr_in ss_addr;
            connect_to_SS_from_client(&ss_sock, &ss_addr, ip_addr, atoi(server_addr));
            if (send(ss_sock, path, sizeof(path), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            char mid_ack[100];
            if (recv(ss_sock, mid_ack, sizeof(mid_ack), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            if (strcmp(mid_ack, "success") != 0)
            {
                printf(RED "%s\n" RESET, mid_ack);
                continue;
            }

            // Getting file contents
            printf("\n");
            char buffer[1024];
            int c = 0;
            while (c == 0)
            {
                recv(ss_sock, buffer, sizeof(buffer), 0);

                if (strcmp("DONE", buffer) == 0 || strcmp("DONE\n", buffer) == 0)
                {
                    c = 2; // DONE WITH FILE
                    printf(CYAN "\nFinished reading file!\n" RESET);
                    break;
                }
                else
                {
                    printf(PINK "%s" RESET, buffer);
                }
            }
            printf("\n");
            close_socket(&ss_sock);
        }
        else if (option == 7) // Permissions
        {
            if (send(naming_server_sock, "7", strlen("7"), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }
            char path[MAX_FILE_PATH];
            printf(CYAN "Enter the path: " RESET);
            scanf("%s", path);

            if (send(naming_server_sock, path, sizeof(path), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

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
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }

            char mid_ack[100];
            if (recv(ns_sock, mid_ack, sizeof(mid_ack), 0) == -1)
            {
                printf(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            if (strcmp(mid_ack, "success") != 0)
            {
                printf(RED "%s\n" RESET, mid_ack);
                continue;
            }

            char permission[1024];

            if (recv(ns_sock, permission, sizeof(permission), 0) == -1)
            {
                perror(RED "Error receiving data" RESET);
                exit(0);
            }
            if (strcmp(permission, "failed") == 0)
            {
                printf(RED "File does not exist\n" RESET);
                continue;
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