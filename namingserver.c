#include "header.h"

int main()
{
    Tree SS1 = MakeNode(".");
    storage_server_list = NULL;
    // storage_server_list->files_and_dirs = SS1;
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size, ss_addr_size;
    int ss_sock; // Socket for the accepted connection
    struct sockaddr_in ss_addr;
    ss_addr_size = sizeof(ss_addr);

    open_naming_server_port(5566, &server_sock, &server_addr);

    // we now have a dedicated port for the naming server
    int ns_sock;
    struct sockaddr_in ns_addr;

    // storage_servers list = NULL;
    if (initialize_SS(&server_sock, &client_sock, &ns_sock, &client_addr, &ns_addr, &addr_size) == -1)
    {
        printf(RED "[-]Error initializing storage servers\n" RESET);
        return 1;
    }

    while (1)
    {
        PrintAll();
        char opt[2];
        int recieved;
        if ((recieved = recv(client_sock, &opt, sizeof(opt), 0)) == -1)
        {
            perror(RED "Not successful" RESET);
            close(client_sock);
            exit(0);
        }
        else if (recieved == 0)
        {
            // The client has closed the connection, so break out of the loop
            printf(RED "Client disconnected.\n" RESET);
            close(client_sock);
            break;
        }
        opt[strlen(opt)] = '\0';

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
                perror(RED "Not successful" RESET);
                exit(0);
            }
            else
                success[strlen(success)] = '\0';

            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Deleted Successfully!\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                    printf(RED "[-]Send error\n" RESET);
            }
            else
            {
                printf(RED "[-]Deletion unsuccessful\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                    printf(RED "[-]Send error\n" RESET);
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
                perror(RED "Not successful" RESET);
                return 1;
            }

            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1)
            {
                printf(RED "[-]Send error\n" RESET);
                return 1;
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
                printf(RED "[-]Send error\n" RESET);

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1)
                printf(RED "[-]Send error\n" RESET);

            //  Sending option to the SS
            if (send(ns_sock, create_option, sizeof(create_option), 0) == -1)
                printf(RED "[-]Send error\n" RESET);

            // Checking if creation was successful
            char success[25];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1)
            {
                perror(RED "[-]Creation unsuccessful\n" RESET);
                return 1;
            }
            else
                success[strlen(success)] = '\0';

            if (strcmp(success, "done") == 0)
            {
                printf(GREEN "Created Successfully!\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                    printf(RED "[-]Send error\n" RESET);
            }
            else
            {
                printf(RED "[-] %s\n" RESET, success);
                perror(RED "[-]Creation unsuccessful\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1)
                    printf(RED "[-]Send error\n" RESET);
            }
        }
        else if (strcmp("4", opt) == 0)
        {
        }
        else if (strcmp("5", opt) == 0) // Write
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "5", sizeof("5"), 0) == -1)
                printf("[-]Send error\n");
            close_socket(&ns_sock);

            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror("Not successful");
                exit(0);
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
                    printf(RED "[-] Send error\n" RESET);
                flag = 1;
                continue;
            }

            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                    printf(RED "[-] Send error\n" RESET);
                if (send(client_sock, server, sizeof(server), 0) == -1)
                {
                    printf(RED "[-] Send error\n" RESET);
                }
            }
        }
        else if (strcmp("6", opt) == 0) // Read
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "6", sizeof("6"), 0) == -1)
                printf(RED "[-]Send error\n" RESET);
            close_socket(&ns_sock);

            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                perror(RED "Not successful" RESET);
                exit(0);
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
                    printf(RED "[-] Send error\n" RESET);
                flag = 1;
                continue;
            }

            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                    printf(RED "[-] Send error\n" RESET);
                if (send(client_sock, server, sizeof(server), 0) == -1)
                    printf(RED "[-] Send error\n" RESET);
            }
        }
        else if (strcmp("7", opt) == 0) // Permissions
        {
            connect_to_SS_from_NS(&ns_sock, &ns_addr, 5566);
            if (send(ns_sock, "7", sizeof("7"), 0) == -1)
                printf(RED "[-]Send error\n" RESET);
            close_socket(&ns_sock);

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
                    printf(RED "[-] Send error\n" RESET);
                flag = 1;
                continue;
            }
            
            if (flag == 0)
            {
                if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1)
                    printf(RED "[-] Send error\n" RESET);
                if (send(client_sock, server, sizeof(server), 0) == -1)
                {
                    printf(RED "[-] Send error\n" RESET);
                }
            }
        }
    }
    close_socket(&ns_sock);
    close_socket(&client_sock);
    close_socket(&server_sock);

    return 0;
}