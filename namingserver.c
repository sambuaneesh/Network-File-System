#include "header.h"

int main()
{
    Tree SS1 = MakeNode(".");
    storage_server_list = NULL;
    // storage_server_list->files_and_dirs = SS1;
    int nm_sock, client_sock, ss_sock;
    struct sockaddr_in server_addr, client_addr, ss_addr;
    socklen_t client_addr_size, ss_addr_size;

    ss_addr_size = sizeof(ss_addr);

    open_naming_server_port(5566, &nm_sock, &server_addr);
    make_socket_non_blocking(nm_sock); // so that some accept requests can be ignored

    // we now have a dedicated port for the naming server
    int ns_sock;
    struct sockaddr_in ns_addr;

    // storage_servers list = NULL;
    // NISHITA
    // if (initialize_SS(&nm_sock, &client_sock, &ns_sock, &client_addr, &ns_addr, &addr_size) == -1)
    // {
    //     printf(RED "[-]Error initializing storage servers\n" RESET);
    //     return 1;
    // }
    // NISHITA
    int something_connect = 0;
    int num_ss = 0;
    int num_client = 0;
    int role = 0;

    while (1)
    {
        // NISHITA
        ss_sock = accept(nm_sock, (struct sockaddr *)&ss_addr, &ss_addr_size);
        if (ss_sock < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                // No client connection available, continue with other tasks
            }
            else
            {
                perror(RED "[-]Accept error" RESET);
                exit(0);
            }
        }
        else
        {
            if (recv(ss_sock, &role, sizeof(role), 0) == -1)
            {
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }
            else
            {
                printf("role is %d\n", role);
                something_connect = 1;
                if (role == 1)
                {
                    num_ss++;
                    if (initialize_SS(&ss_sock) == -1)
                    {
                        perror(RED "[-]Error initializing storage servers" RESET);
                        exit(1);
                    }
                    printf("[+]New storage server connected\n");
                    close_socket(&ss_sock);
                    continue;
                }
                else if(role == 2)
                {
                    client_sock = ss_sock;
                    client_addr = ss_addr;
                    client_addr_size = ss_addr_size;
                    num_client++;
                    printf("[+]New client connected\n");
                }
            }
            // receive vital information, store in ll, disconnect
        }
        if (something_connect == 0 || (something_connect != 0 && num_client == 0))
        {
            continue;
        }
        if (num_ss == 0 && num_client != 0)
        {
            printf(RED "[-]No storage servers connected\n" RESET);
            break;
            continue;
        }

        PrintAll();
        char opt[2];
        int recieved;
        if ((recieved = recv(client_sock, &opt, sizeof(opt), 0)) == -1)
        {
            perror(RED "[-]Receive Error" RESET);
            close(client_sock);
            exit(1);
        }
        else if (recieved == 0)
        {
            // The client has closed the connection, so break out of the loop
            printf(RED "Client disconnected.\n" RESET);
            close(client_sock);
            break;
        }
        opt[strlen(opt)]= '\0';

        if (strcmp("1", opt) == 0)
        {
        }
        else if (strcmp("2", opt) == 0) // Deletion
        {
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "Not successful" RESET);
                exit(0);
            }

            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1)
            {
                perror(RED "Not successful" RESET);
                exit(0);
            }

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS
            Tree T = check_if_path_in_ss(file_path, 0);
            printf("T is %p\n", T);
            if (Delete_Path(T, file_path) == -1)
            {
                printf(RED "[-]Path not in list of accessible paths\n" RESET);
            }
            printf("Waiting for success message\n");

            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "2", sizeof("2"), 0) == -1)
                printf(RED "[-]Send error\n" RESET);

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1)
                printf(RED "[-]Send error\n" RESET);

            //  Sending option to the SS

            if (send(ns_sock, create_option, sizeof(create_option), 0) == -1)
                printf(RED "[-]Send error\n" RESET);

            // Checking if creation was successful
            char success[20];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1)
            {
                perror(RED "[-]Not successful" RESET);
                exit(0);
            }
            else
                success[strlen(success)]= '\0';

            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Deleted Successfully!\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            else
            {
                printf(RED "[-]Deletion unsuccessful\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
        else if (strcmp("3", opt) == 0) // Creation
        {
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1)
            {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS

            if (check_if_path_in_ss(file_path, 1) == NULL)
            {
                printf(RED "[-]Path not in list of accessible paths\n" RESET);
                continue;
            }

            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "3", sizeof("3"), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            //  Sending option to the SS
            if (send(ns_sock, create_option, sizeof(create_option), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Checking if creation was successful
            char success[25];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1)
            {
                perror(RED "[-]Creation unsuccessful\n" RESET);
                return 1;
            }
            else
                success[strlen(success)]= '\0';

            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Created Successfully!\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            else
            {
                printf(RED "[-]%s\n" RESET, success);
                perror(RED "[-]Creation unsuccessful\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
        else if (strcmp("4", opt) == 0)
        {
        }
        else if (strcmp("5", opt) == 0) // Write
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "5", sizeof("5"), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            // Not sure what to do with this:
            // Do I keep some kind of while loop to search for the server with the mentioned path
            // and then use those ports and all?
            storage_servers temp = MakeNode_ss("127.0.0.1", 5568, 5568);

            int server_addr = temp->ss_send->server_port;
            char ip_addr[50];
            strcpy(ip_addr, temp->ss_send->ip_addr);
            char server[50];
            snprintf(server, sizeof(server), "%d", server_addr);
            int flag = 0;

            if (check_if_path_in_ss(file_path, 0) == NULL)
            {
                printf(RED "[-]Path not in list of accessible paths\n" RESET);
                if (send(client_sock, "failed", sizeof("failed"), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                flag = 1;
                char success_msg[100];
                strcpy(success_msg, "fail");
                if (send(ns_sock, success_msg, sizeof(success_msg), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }

                close_socket(&ns_sock);
                continue;
            }
            else
            {
                if (send(ns_sock, "success", sizeof("success"), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }
            }
            close_socket(&ns_sock);

            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                if (send(client_sock, server, sizeof(server), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
        else if (strcmp("6", opt) == 0) // Read
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "6", sizeof("6"), 0) == -1)
            {
                perror(RED "[-]Send error" RESET);
                exit(1);
            }
            // close_socket(&ns_sock);

            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }

            // Not sure what to do with this:
            // Do I keep some kind of while loop to search for the server with the mentioned path
            // and then use those ports and all?
            storage_servers temp = MakeNode_ss("127.0.0.1", 5568, 5568);

            int server_addr = temp->ss_send->server_port;
            char ip_addr[50];
            strcpy(ip_addr, temp->ss_send->ip_addr);
            char server[50];
            snprintf(server, sizeof(server), "%d", server_addr);

            int flag = 0;

            if (check_if_path_in_ss(file_path, 0) == NULL)
            {
                printf(RED "[-]Path not in list of accessible paths\n" RESET);
                if (send(client_sock, "failed", sizeof("failed"), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                flag = 1;
                char success_msg[100];
                strcpy(success_msg, "fail");
                if (send(ns_sock, success_msg, sizeof(success_msg), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }

                close_socket(&ns_sock);
                continue;
            }
            else
            {
                if (send(ns_sock, "success", sizeof("success"), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }
            }
            close_socket(&ns_sock);
            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                {
                    printf(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                if (send(client_sock, server, sizeof(server), 0) == -1)
                {
                    printf(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
        else if (strcmp("7", opt) == 0) // Permissions
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "7", sizeof("7"), 0) == -1)
            {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "Not successful" RESET);
                exit(0);
            }

            storage_servers temp = MakeNode_ss("127.0.0.1", 5568, 5568);

            int server_addr = temp->ss_send->server_port;
            char ip_addr[50];
            strcpy(ip_addr, temp->ss_send->ip_addr);
            char server[50];
            snprintf(server, sizeof(server), "%d", server_addr);

            int flag = 0;

            if (check_if_path_in_ss(file_path, 0) == NULL)
            {
                printf(RED "[-]Path not in list of accessible paths\n" RESET);
                if (send(client_sock, "failed", sizeof("failed"), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                flag = 1;

                char success_msg[100];
                strcpy(success_msg, "fail");
                if (send(ns_sock, success_msg, sizeof(success_msg), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }

                close_socket(&ns_sock);
                continue;
            }
            else
            {
                if (send(ns_sock, "success", sizeof("success"), 0) == -1)
                {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }
            }
            close_socket(&ns_sock);

            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
                if (send(client_sock, server, sizeof(server), 0) == -1)
                {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
    }
    close_socket(&ns_sock);
    close_socket(&client_sock);
    close_socket(&nm_sock);

    return 0;
}