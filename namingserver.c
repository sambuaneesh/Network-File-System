#include "header.h"

int main()
{
    Tree SS1 = MakeNode(".");
    storage_server_list = MakeNode_ss(" ", 1, 2);
    storage_server_list->files_and_dirs = SS1;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size, ss_addr_size;
    int ss_sock; // Socket for the accepted connection
    struct sockaddr_in ss_addr;
    ss_addr_size = sizeof(ss_addr);

    open_naming_server_port(5566, &server_sock, &server_addr);
    // we now have a dedicated port for the naming server

    storage_servers list = NULL;
    int num_storage_servers = 0;
    listen_for_client(&server_sock, &client_sock, &client_addr, &addr_size);
    load_SS(SS1, "paths.txt");

    int ns_sock;
    struct sockaddr_in ns_addr;
    while (1)
    {
        PrintTree(SS1);
        char opt[2];
        int recieved;
        if ((recieved = recv(client_sock, &opt, sizeof(opt), 0)) == -1)
        {
            perror("Not successful");
            close(client_sock);
            exit(0);
        }
        else if (recieved == 0)
        {
            // The client has closed the connection, so break out of the loop
            printf("Client disconnected.\n");
            close(client_sock);
            break;
        }
        opt[strlen(opt)] = '\0';

        if (strcmp("1", opt) == 0)
        {
        }
        else if (strcmp("2", opt) == 0) // Deletion
        {
            // printf("Deletion\n");
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];
            char server_number[10];
            // Recieving the server number
            if ((recieved = recv(client_sock, &server_number, sizeof(server_number), 0)) == -1)
            {
                perror("Not successful");
                close(client_sock);
                exit(0);
            }

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
            }

            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
            }

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS
            Delete_Path(SS1, file_path);

            connect_to_SS_from_NS(&ns_sock, &ns_addr);
            if (send(ns_sock, "2", sizeof("2"), 0) == -1)
                printf("[-]Send error\n");

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1)
                printf("[-]Send error\n");

            //  Sending option to the SS

            if (send(ns_sock, create_option, sizeof(create_option), 0) == -1)
                printf("[-]Send error\n");

            // Checking if creation was successful
            char success[5];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
            }
            else
            {
                success[strlen(success)] = '\0';
            }

            if (strcmp(success, "done") == 0)
            {
                Delete_from_path_file(file_path);
                printf("Deleted Successfully!\n");
            }
            else
            {
                printf(RED "[-]Deletion unsuccessful\n" RESET);
            }
        }
        else if (strcmp("3", opt) == 0) // Creation
        {
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];
            char server_number[10];
            // Recieving the server number
            if ((recieved = recv(client_sock, &server_number, sizeof(server_number), 0)) == -1)
            {
                perror("Not successful");
                close(client_sock);
                exit(0);
            }

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
            }

            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1)
            {
                // perror("Not successful");
                printf(RED "[-]Send error\n" RESET);
                // exit(0);
                continue;
            }

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS

            if (check_if_path_in_ss(file_path) == -1)
            {
                printf("[-]Path not in list of accessible paths\n");
                continue;
            }

            connect_to_SS_from_NS(&ns_sock, &ns_addr);
            if (send(ns_sock, "3", sizeof("3"), 0) == -1)
                printf("[-]Send error\n");

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1)
                printf("[-]Send error\n");

            //  Sending option to the SS

            if (send(ns_sock, create_option, sizeof(create_option), 0) == -1)
                printf("[-]Send error\n");

            // Checking if creation was successful
            char success[5];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
            }
            else
            {
                success[strlen(success)] = '\0';
            }

            if (strcmp(success, "done") == 0)
            {
                if (Add_to_path_file(file_path) == 0)
                    printf("Created Successfully!\n");
                else
                    return 1;
            }
        }
    }
    close_socket(&ns_sock);
    close_socket(&client_sock);
    close_socket(&server_sock);
    return 0;
}