#include "header.h"

int something_connect = 0;
int num_ss            = 0;
int num_client        = 0;
int role              = 0;

// creating a thread for client connection
void* client_thread(void* arg)
{
    int client_sock, nm_sock, ss_sock, ns_sock;
    struct sockaddr_in client_addr, server_addr, ss_addr, ns_addr;
    socklen_t client_addr_size, ss_addr_size, ns_addr_size;
    Cache cache;

    if (arg != NULL) {
        struct client_thread_args* args = (struct client_thread_args*)arg;
        client_sock                     = args->client_sock;
        client_addr                     = args->client_addr;
        client_addr_size                = args->client_addr_size;
        cache                           = args->cache;
        nm_sock                         = args->nm_sock;
        server_addr                     = args->server_addr;
        ss_sock                         = args->ss_sock;
        ss_addr                         = args->ss_addr;
        ss_addr_size                    = args->ss_addr_size;
        ns_sock                         = args->ns_sock;
        ns_addr                         = args->ns_addr;
    }
    else {
        printf(RED "[-]Error in passing arguments to client thread\n" RESET);
        pthread_exit(NULL);
    }


    while (1) {
        PrintAll();
        char opt[2];
        int recieved;
        if ((recieved = recv(client_sock, &opt, sizeof(opt), 0)) == -1) {
            perror(RED "[-]Receive Error" RESET);
            close(client_sock);
            exit(1);
        }
        else if (recieved == 0) {
            // The client has closed the connection, so break out of the loop
            printf(RED "Client disconnected.\n" RESET);
            close(client_sock);
            break;
        }
        opt[recieved] = '\0';
        printf("option %s\n", opt);

        if (strcmp("1", opt) == 0) {
            close_socket(&client_sock);
        }
        else if (strcmp("2", opt) == 0)// Deletion
        {
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1) {
                perror(RED "Not successful" RESET);
                exit(1);
            }
            file_path[recieved] = '\0';
            // Recieving the create option - 1 for file and 2 for directory
            char delete_option[10];
            if ((recieved = recv(client_sock, &delete_option, sizeof(delete_option), 0)) == -1) {
                perror(RED "Not successful" RESET);
                exit(0);
            }
            delete_option[recieved] = '\0';

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS
            char mid_mess[100];

            storage_servers storage_server_details = CheckCache(cache, opt, file_path, "\0");
            if (storage_server_details == NULL) {
                storage_server_details = check_if_path_in_ss(file_path, 0);
                if (storage_server_details == NULL) {
                    strcpy(mid_mess, INVALID_PATH);
                    if (send(client_sock, mid_mess, sizeof(mid_mess), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    printf(RED "%s\n" RESET, INVALID_PATH);
                    continue;
                }
            }

            if (storage_server_details != NULL) {
                strcpy(mid_mess, "success");
                if (send(client_sock, mid_mess, sizeof(mid_mess), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }

            connect_to_SS_from_NS(&ns_sock, &ns_addr, storage_server_details->ss_send->server_port);
            if (send(ns_sock, "2", sizeof("2"), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Sending path to the SS
            if (send(ns_sock, file_path, sizeof(file_path), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            //  Sending option to the SS

            if (send(ns_sock, delete_option, sizeof(delete_option), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Checking if creation was successful
            char success[100];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1) {
                perror(RED "[-]Not successful" RESET);
                exit(0);
            }
            else {
                success[success_message] = '\0';
            }

            if (strcmp(success, "done") == 0) {
                printf(GREEN "Deleted Successfully!\n" RESET);
                if (Delete_Path(storage_server_details->files_and_dirs,
                                file_path,
                                storage_server_details->files_and_dirs->path)
                    == -1) {
                    if (send(client_sock, "failed", sizeof("failed"), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    printf(RED "%s\n" RESET, INVALID_PATH);
                    continue;
                }
                if (send(client_sock, success, sizeof(success), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            else {
                printf(RED "%s\n" RESET, success);
                if (send(client_sock, success, sizeof(success), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            close_socket(&ns_sock);
        }
        else if (strcmp("3", opt) == 0)// Creation
        {
            char temp_file_path[MAX_FILE_PATH];
            char temp_option[10];

            // Receiving the path of the file/directory
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1) {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            file_path[recieved] = '\0';

            char file_path_dest[MAX_FILE_PATH];
            strcpy(file_path_dest, file_path);
            file_path_dest[recieved] = '\0';
            // Recieving the create option - 1 for file and 2 for directory
            char create_option[10];
            if ((recieved = recv(client_sock, &create_option, sizeof(create_option), 0)) == -1) {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }
            create_option[recieved] = '\0';

            char temp_opt[10];
            strcpy(temp_opt, create_option);
            temp_opt[recieved] = '\0';
            char succ_mess[100];

            // END OF GETTING DATA FROM CLIENT
            // THE REST OF THIS CODE MUST EXECUTE ONLY IF file_path IS IN THE LIST OF ACCESSIBLE PATHS

            storage_servers storage_server_details = CheckCache(cache, opt, file_path, "\0");
            if (storage_server_details == NULL) {
                storage_server_details = find_ss(file_path);
                if (storage_server_details == NULL) {
                    strcpy(succ_mess, INVALID_PATH);
                    // printf("2. MIDMES: %s\n",succ_mess);
                    if (send(client_sock, INVALID_PATH, sizeof(INVALID_PATH), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        // exit(1);
                        pthread_exit(NULL);
                    }

                    printf(RED "%s\n" RESET, INVALID_PATH);
                    continue;
                }
            }

            if (storage_server_details != NULL) {
                strcpy(succ_mess, "success");

                if (send(client_sock, "success", sizeof("success"), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }

            connect_to_SS_from_NS(&ns_sock, &ns_addr, storage_server_details->ss_send->server_port);
            if (send(ns_sock, "3", sizeof("3"), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Sending path to the SS
            if (send(ns_sock, file_path_dest, sizeof(file_path_dest), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            //  Sending option to the SS
            if (send(ns_sock, temp_opt, sizeof(temp_opt), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }

            // Checking if creation was successful
            char success[100];
            int success_message = 0;

            if ((success_message = recv(ns_sock, &success, sizeof(success), 0)) == -1) {
                perror(RED "[-]Receive error\n" RESET);
                // return 1;
                pthread_exit(NULL);
            }
            else {
                success[success_message] = '\0';
            }
            //printf("SUCCESS: %s\n", success);
            if (strcmp(success, "done") == 0) {
                printf(GREEN "Created Successfully!\n" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            else {
                printf(RED "[-]%s\n" RESET, success);
                perror(RED "[-]Creation unsuccessful" RESET);
                if (send(client_sock, success, sizeof(success), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            close_socket(&ns_sock);
        }
        else if (strcmp("4", opt) == 0)// Copying files/directories
        {
            // Receiving the path of the file/directory
            char source_path[MAX_FILE_PATH];
            char dest_path[MAX_FILE_PATH];
            char source_full_path[MAX_FILE_PATH];
            char dest_full_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &source_path, sizeof(source_path), 0)) == -1) {
                perror(RED "Not successful" RESET);
                exit(0);
            }
            if ((recieved = recv(client_sock, &dest_path, sizeof(dest_path), 0)) == -1) {
                perror(RED "Not successful" RESET);
                exit(0);
            }

            // Recieving the create option - 1 for file and 2 for directory
            char buffer_paths[MAX_FILE_PATH];
            char copy_option[10];
            if ((recieved = recv(client_sock, &copy_option, sizeof(copy_option), 0)) == -1) {
                perror(RED "Not successful" RESET);
                exit(0);
            }

            //  Checking if destination is accessible
            storage_servers storage_server_details = check_if_path_in_ss(source_path, 0);
            if (storage_server_details == NULL) {
                if (send(client_sock, INVALID_PATH, sizeof(INVALID_PATH), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }

                printf(RED "%s\n" RESET, INVALID_PATH);
                continue;
            }

            strcpy(source_full_path, storage_server_details->files_and_dirs->path);
            strcat(source_full_path, source_path);

            // Checking if source is accessible
            storage_server_details = check_if_path_in_ss(dest_path, 0);
            if (storage_server_details == NULL) {
                if (send(client_sock, INVALID_PATH, sizeof(INVALID_PATH), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }

                printf(RED "%s\n" RESET, INVALID_PATH);
                continue;
            }


            strcpy(dest_full_path, storage_server_details->files_and_dirs->path);
            strcat(dest_full_path, dest_path);
            strcpy(buffer_paths, "");

            //Checking if destination path is a file
            struct stat fileStat;
            char mid_ack[100];

            struct stat fileStat1;

            if (stat(source_full_path, &fileStat1) == 0) {
                if (S_ISREG(fileStat1.st_mode) && strcmp(copy_option, "2") == 0) {
                    if (send(client_sock, DIR_OPT, sizeof(DIR_OPT), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    continue;
                }
                else if (S_ISDIR(fileStat1.st_mode) && strcmp(copy_option, "1") == 0) {
                    if (send(client_sock, FILE_OPT, sizeof(FILE_OPT), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    continue;
                }
            }


            int fil = 0;
            if (stat(dest_full_path, &fileStat) == 0) {
                if (S_ISREG(fileStat.st_mode)) {
                    if (send(client_sock, COPY_TO_FILE, sizeof(COPY_TO_FILE), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    continue;
                }
                else {

                    if (send(client_sock, "success", sizeof("success"), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                }
            }

            int error    = 1;
            char* buffer = (char*)malloc(sizeof(char) * 1500);
            strcpy(buffer, "");

            if (strcmp(copy_option, "1") == 0) {
                error = copy_file(source_full_path, dest_full_path, buffer_paths);
                buffer_paths[strlen(buffer_paths)] = '\0';
                if (error != 0) {
                    connect_to_SS_from_NS(&ns_sock,
                                          &ns_addr,
                                          storage_server_details->ss_send->server_port);
                    if (send(ns_sock, "4", sizeof("4"), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    if (send(ns_sock, buffer_paths, sizeof(buffer_paths), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }

                    storage_servers storage_server_details = check_if_path_in_ss(buffer_paths, 1);
                    close_socket(&ns_sock);
                }
                else {
                    fil = 1;
                }
            }
            else if (strcmp(copy_option, "2") == 0) {
                char temp[1000];
                int temp_ind = 0;
                int i        = 0;
                for (i = strlen(source_path) - 1; i >= 0; i--) {
                    if (source_path[i] == '/') {
                        i++;
                        break;
                    }
                }
                for (int j = i; j < strlen(source_path); j++) {
                    temp[temp_ind] = source_path[j];
                    temp_ind++;
                }
                temp[temp_ind]  = '\0';
                char* temp_dest = (char*)malloc(sizeof(char) * 1000);

                strcpy(temp_dest, dest_full_path);
                strcat(temp_dest, "/");
                strcat(temp_dest, temp);

                strcat(buffer_paths, "\n");
                strcpy(buffer_paths, dest_path);
                strcat(buffer_paths, "/");
                strcat(buffer_paths, temp);
                strcat(buffer_paths, "\n");

                error = copy_directory(source_full_path,
                                       temp_dest,
                                       buffer_paths,
                                       dest_full_path,
                                       dest_path);
                if (error != 0) {
                    connect_to_SS_from_NS(&ns_sock,
                                          &ns_addr,
                                          storage_server_details->ss_send->server_port);
                    if (send(ns_sock, "4", sizeof("4"), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    printf("BUFFER: %s END\n", buffer_paths);
                    buffer_paths[strlen(buffer_paths)] = '\0';
                    if (send(ns_sock, buffer_paths, sizeof(buffer_paths), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    // Adding every path in buffer_paths to the tree
                    char* token;
                    char* saveptr;

                    // Use strtok_r to tokenize the buffer based on newline character
                    token = strtok_r(buffer_paths, "\n", &saveptr);
                    while (token != NULL) {


                        // Assuming find_ss is a function that takes a const char* as an argument
                        storage_servers storage_server_details = check_if_path_in_ss(token, 1);

                        token = strtok_r(NULL, "\n", &saveptr);
                    }

                    close_socket(&ns_sock);
                }
                else {
                    fil = 2;
                }
            }
            if (error == 0) {
                if (fil == 1) {
                    if (send(client_sock, FILE_EXISTS, sizeof(FILE_EXISTS), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                }
                else {
                    if (send(client_sock, DIR_EXISTS, sizeof(DIR_EXISTS), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                }
            }
            else {
                if (send(client_sock, "done", sizeof("done"), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
        }
        else if (strcmp("5", opt) == 0 || strcmp("6", opt) == 0 || strcmp("7", opt) == 0)// Write
        {
            char file_path[MAX_FILE_PATH];
            if ((recieved = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1) {
                perror(RED "[-]Receive error\n" RESET);
                exit(1);
            }

            char mid_ack1[100];
            storage_servers storage_server_details = CheckCache(cache, opt, file_path, "\0");
            if (storage_server_details == NULL) {
                storage_server_details = check_if_path_in_ss(file_path, 0);
                if (storage_server_details == NULL) {
                    printf(RED "%s\n" RESET, INVALID_PATH);
                    strcpy(mid_ack1, INVALID_PATH);
                    if (send(client_sock, mid_ack1, sizeof(mid_ack1), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    continue;
                }
                else {
                    strcpy(mid_ack1, "success");
                    if (send(client_sock, mid_ack1, sizeof(mid_ack1), 0) == -1) {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                }
            }
            else {
                strcpy(mid_ack1, "success");
                if (send(client_sock, mid_ack1, sizeof(mid_ack1), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }


            int server_addr = storage_server_details->ss_send->client_port;
            char ip_addr[50];
            strcpy(ip_addr, storage_server_details->ss_send->ip_addr);
            char server[50];
            snprintf(server, sizeof(server), "%d", server_addr);

            if (send(client_sock, ip_addr, sizeof(ip_addr), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }
            if (send(client_sock, server, sizeof(server), 0) == -1) {
                perror(RED "[-]Send error\n" RESET);
                exit(1);
            }


            connect_to_SS_from_NS(&ns_sock, &ns_addr, storage_server_details->ss_send->server_port);
            if (strcmp("5", opt) == 0) {
                if (send(ns_sock, "5", sizeof("5"), 0) == -1) {
                    perror(RED "[-]Send error\n" RESET);
                    exit(1);
                }
            }
            else if (strcmp("6", opt) == 0) {
                if (send(ns_sock, "6", sizeof("6"), 0) == -1) {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }
            }
            else if (strcmp("7", opt) == 0) {
                if (send(ns_sock, "7", sizeof("7"), 0) == -1) {
                    perror(RED "[-]Send error" RESET);
                    exit(1);
                }
            }

            close_socket(&ns_sock);
        }
    }
}

int main()
{
    Tree SS1            = MakeNode(".");
    storage_server_list = NULL;
    int nm_sock, client_sock, ss_sock;
    struct sockaddr_in server_addr, client_addr, ss_addr;
    socklen_t client_addr_size, ss_addr_size;

    ss_addr_size = sizeof(ss_addr);
    Cache cache  = InitCache();

    open_naming_server_port(5566, &nm_sock, &server_addr);
    make_socket_non_blocking(nm_sock);// so that some accept requests can be ignored

    // we now have a dedicated port for the naming server
    int ns_sock;
    struct sockaddr_in ns_addr;

    while (1) {
        ss_sock = accept(nm_sock, (struct sockaddr*)&ss_addr, &ss_addr_size);
        if (ss_sock < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No client connection available, continue with other tasks
            }
            else {
                perror(RED "[-]Accept error" RESET);
                exit(0);
            }
        }
        else {
            if (recv(ss_sock, &role, sizeof(role), 0) == -1) {
                perror(RED "[-]Receive error" RESET);
                exit(1);
            }
            else {
                printf("role is %d\n", role);
                something_connect = 1;
                if (role == 1)// SS
                {
                    num_ss++;
                    if (initialize_SS(&ss_sock) == -1) {
                        perror(RED "[-]Error initializing storage servers" RESET);
                        exit(1);
                    }
                    printf("[+]New storage server connected\n");
                    close_socket(&ss_sock);
                    continue;
                }
                else if (role == 2)// Client
                {
                    client_sock      = ss_sock;
                    client_addr      = ss_addr;
                    client_addr_size = ss_addr_size;
                    num_client++;
                    printf("[+]New client connected\n");

                    // create a thread for the client connection and pass the client_thread_args
                    // structure as an argument
                    struct client_thread_args args;
                    args.client_sock      = client_sock;
                    args.client_addr      = client_addr;
                    args.client_addr_size = client_addr_size;
                    args.cache            = cache;
                    args.nm_sock          = nm_sock;
                    args.server_addr      = server_addr;
                    args.ss_sock          = ss_sock;
                    args.ss_addr          = ss_addr;
                    args.ss_addr_size     = ss_addr_size;
                    args.ns_sock          = ns_sock;
                    args.ns_addr          = ns_addr;

                    pthread_t client_thread_id;
                    pthread_create(&client_thread_id, NULL, client_thread, (void*)&args);

                    // dont forget to join later
                }
            }
            // receive vital information, store in ll, disconnect
        }
        if (something_connect == 0 || (something_connect != 0 && num_client == 0)) {
            continue;
        }
        if (num_ss == 0 && num_client != 0) {
            printf(RED "[-]No storage servers connected\n" RESET);
            break;
            continue;
        }
    }
    close_socket(&nm_sock);

    return 0;
}

/*

nm
ss1
ss2

nm - ss1
nm - ss2

create ss1
read ss2
write ss2

*/
