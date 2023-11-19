#include "header.h"

int port_for_client;
int port_for_nm;
char ip[16]                    = {'\0'};
char paths_file[MAX_FILE_NAME] = {'\0'};

// create array of semaphore for each file
sem_t sem_array[MAX_NUM_FILES];

FileMapping fileMappings[MAX_NUM_FILES];// Global array to store mappings

// thread to handle the client
void* handleClient(void* args)
{
    struct ss_thread_args* client_args = (struct ss_thread_args*)args;

    // extract the arguments
    char command[2];
    strcpy(command, client_args->command);
    int received = client_args->received;
    char file_path[MAX_FILE_PATH];
    strcpy(file_path, client_args->file_path);
    int naming_server_sock = client_args->naming_server_sock;
    int sock_ss_client     = client_args->sock_ss_client;
    int client_sock        = client_args->client_sock;
    struct sockaddr_in cli_addr;
    cli_addr                = client_args->cli_addr;
    socklen_t cli_addr_size = client_args->cli_addr_size;


    if (strcmp(command, "1")
        == 0)// Must be an invalid option, cause we now use it for exit, right??
    {
        // NISHITA
        // printf("File contents:\n%s\n", buffer);
        // if (send(naming_server_sock, buffer, sizeof(buffer), 0) == -1)
        //     perror(RED "[-] Error sending data" RESET);
        // if (send(naming_server_sock, &port_for_client, sizeof(port_for_client), 0) == -1)
        //     perror(RED "[-] Error sending data" RESET);
        // if (send(naming_server_sock, &port_for_nm, sizeof(port_for_nm), 0) == -1)
        //     perror(RED "[-] Error sending data" RESET);
        // if (send(naming_server_sock, ip, sizeof(ip), 0) == -1)
        //     perror(RED "[-] Error sending data" RESET);
        // NISHITA
    }
    else if (strcmp(command, "2") == 0)// Deletion
    {
        if ((received = recv(naming_server_sock, &file_path, sizeof(file_path), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else {
            file_path[received] = '\0';
        }

        char* temp = (char*)malloc(sizeof(char) * 1000);
        strcpy(temp, file_path);

        char option[10];

        if ((received = recv(naming_server_sock, &option, sizeof(option), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else {
            option[received] = '\0';
        }

        int del = 0;

        char cwd[1000];

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
        }
        else {
            perror(RED "getcwd() error" RESET);
            exit(0);
        }
        strcat(cwd, file_path);

        // If option is 1, delete a file, if option is 2, delete a directory
        if (strcmp(option, "1") == 0) {
            if (delete_file(cwd) == -1) {
                del = 1;
            }
        }
        else if (strcmp(option, "2") == 0) {
            if (delete_directory(cwd) == -1) {
                del = 2;
            }
        }
        // printf("del: %d\n",del);
        char succ_mess[100];

        struct stat fileStat1;

            if (stat(cwd, &fileStat1) == 0) {
                if (S_ISREG(fileStat1.st_mode) && strcmp(option, "2") == 0) {
                    if (send(naming_server_sock, WRONG_DEL_DIR, sizeof(WRONG_DEL_DIR), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                    return NULL;
                }
                else if (S_ISDIR(fileStat1.st_mode) && strcmp(option, "1") == 0) {
                    if (send(naming_server_sock, WRONG_DEL_FILE, sizeof(WRONG_DEL_FILE), 0) == -1)// mid ack
                    {
                        perror(RED "[-]Send error\n" RESET);
                        exit(1);
                    }
                     return NULL;
                }
            }

        if (del == 0 && Delete_from_path_file(file_path, paths_file) == 0) {
            strcpy(succ_mess, "done");
            printf(GREEN "Deleted Successfully!\n" RESET);
            int sent = send(naming_server_sock, succ_mess, sizeof(succ_mess), 0);
            if (sent == -1) {
                perror(RED "[-] Error sending data" RESET);
            }
        }
        else {
            if (del == 1) {
                strcpy(succ_mess, FILE_DEL);
            }
            if (del == 2) {
                strcpy(succ_mess, DIR_DEL);
            }
            perror(RED "[-] Error deleting file/directory" RESET);
            printf(RED "%s\n" RESET, succ_mess);
            int sent = send(naming_server_sock, succ_mess, sizeof(succ_mess), 0);
            if (sent == -1) {
                perror(RED "[-] Error sending data" RESET);
            }
        }
    }
    else if (strcmp(command, "3") == 0)// Creation
    {
        if ((received = recv(naming_server_sock, &file_path, sizeof(file_path), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else {
            file_path[received] = '\0';
        }

        char* temp = (char*)malloc(sizeof(char) * 1000);
        strcpy(temp, file_path);


        char option[10];
        // memset(option,sizeof(option),0);

        if ((received = recv(naming_server_sock, &option, sizeof(option), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else {
            option[received] = '\0';
        }

        char cwd[MAX_FILE_PATH];

        char buffer_path[MAX_FILE_PATH];
        strcpy(buffer_path, "/");
        get_full_path(temp, buffer_path);
        //  printf("BUFF: %s %s\n",buffer_path,temp);

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
        }
        else {
            perror(RED "getcwd() error" RESET);
            exit(0);
        }
        strcat(cwd, buffer_path);
        //  printf("PATH: %s\n",cwd);

        // If option is 1, create a file, if option is 2, create a directory
        if (strcmp(option, "1") == 0) {
            if (create_file(cwd) == -1) {
                int sent = send(naming_server_sock, FILE_EXISTS, sizeof(FILE_EXISTS), 0);
                if (sent == -1) {
                    perror(RED "Error sending data" RESET);
                }
                return NULL;
            }
        }
        else if (strcmp(option, "2") == 0) {
            if (create_directory(cwd) == -1) {
                int sent = send(naming_server_sock, DIR_EXISTS, sizeof(DIR_EXISTS), 0);
                if (sent == -1) {
                    perror("Error sending data");
                }
                return NULL;
            }
        }
        if (Add_to_path_file(buffer_path, paths_file) == 0) {
            printf(GREEN "Created Successfully!\n" RESET);
        }
        else {
            perror(RED "[-] Error creating file/directory" RESET);
        }

        // Sending success message
        char suc[100];
        strcpy(suc, "done");
        int sent = send(naming_server_sock, suc, sizeof(suc), 0);
        if (sent == -1) {
            perror(RED "Error sending data" RESET);
        }
    }
    else if (strcmp(command, "4") == 0)// Copying files and dirs
    {
        if ((received = recv(naming_server_sock, &file_path, sizeof(file_path), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else {
            file_path[received] = '\0';
        }
        //  printf("FILE: %s\n",file_path);
        if (Add_to_path_file(file_path, paths_file) == 0) {
            printf(GREEN "Copied Successfully!\n" RESET);
        }
    }
    else if (strcmp(command, "5") == 0)// Writing
    {
        if ((client_sock = accept(sock_ss_client, (struct sockaddr*)&cli_addr, &cli_addr_size))
            == -1) {
            perror(RED "[-] Accept error" RESET);
            exit(0);
        }
        else {
            printf("[+]Client connected.\n");
        }


        char file_path[MAX_FILE_PATH];
        // Getting file path from client
        if ((received = recv(client_sock, file_path, sizeof(file_path), 0)) == -1) {
            perror(RED "[-] Receive error" RESET);
            exit(0);
        }

        // map the file path to a unique number
        unsigned int uniqueNumber = mapToRange(file_path);

        // Check if the file is already open
        if (sem_trywait(&sem_array[uniqueNumber]) == -1) {
            // The file is already open
            perror(RED "[-] The file is already open" RESET);
            // send error message to client
            char err_mess[100];
            strcpy(err_mess, "Resource busy");
            if (send(client_sock, err_mess, sizeof(err_mess), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                // exit the thread
                pthread_exit(NULL);
            }
            pthread_exit(NULL);
        }

        char cwd[1000];

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
        }
        else {
            perror(RED "getcwd() error" RESET);
            exit(0);
        }
        strcat(cwd, file_path);

        struct stat fileStat;
        if (stat(cwd, &fileStat) == -1) {
            perror("Error getting file information");
            exit(0);
        }

        // Check if it is a directory
        char succ_mess[100];
        if (S_ISDIR(fileStat.st_mode)) {
            strcpy(succ_mess, DIR_WRITE);
            printf(RED "%s\n" RESET, DIR_WRITE);
            if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
                printf(RED "[-] Error sending data\n" RESET);
                exit(0);
            }

            return NULL;
        }

        FILE* file;
        if ((file = fopen(cwd, "w")) == NULL) {
            perror(RED "[-] Could not open the file" RESET);
            // return 1;
        }
        else {
            strcpy(succ_mess, "success");
            if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                exit(0);
            }
        }
        int received_to_write = 0;
        char received_data_to_write[1024];

        while (1) {
            received_to_write =
                recv(client_sock, received_data_to_write, sizeof(received_data_to_write), 0);

            if (received_to_write == -1) {
                perror(RED "[-] Receive error" RESET);
                break;
            }

            if (received_to_write == 0) {
                // The client closed the connection
                break;
            }

            // Check if the received data indicates the end of the transfer
            if (strcmp(received_data_to_write, "done") == 0
                || strcmp(received_data_to_write, "done\n") == 0) {
                break;
            }

            // Check if the received data is valid before writing to the file
            int valid_data = 1;
            for (size_t i = 0; i < strlen(received_data_to_write); i++) {
                if (!isprint(received_data_to_write[i])) {
                    valid_data = 0;
                    break;
                }
            }

            if (valid_data && strlen(received_data_to_write) >= 1) {
                fprintf(file, "%s\n", received_data_to_write);
            }
        }

        fclose(file);
        sem_post(&sem_array[uniqueNumber]);
    }
    else if (strcmp(command, "6") == 0)// Reading
    {
        if ((client_sock = accept(sock_ss_client, (struct sockaddr*)&cli_addr, &cli_addr_size))
            == -1) {
            perror(RED "[-] Accept error");
            exit(0);
        }
        else {
            printf("[+]Client connected.\n");
        }

        char file_path[100];
        // Getting file path from client
        if ((received = recv(client_sock, file_path, sizeof(file_path), 0)) == -1) {
            printf(RED "Error receiving data\n");
            exit(0);
        }

        // map the file path to a unique number
        unsigned int uniqueNumber = mapToRange(file_path);

        // Check if the file is already open
        if (sem_trywait(&sem_array[uniqueNumber]) == -1) {
            // The file is already open
            perror(RED "[-] The file is already open" RESET);
            // send error message to client
            char err_mess[100];
            strcpy(err_mess, "Resource busy");
            if (send(client_sock, err_mess, sizeof(err_mess), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                // exit the thread
                pthread_exit(NULL);
            }
            // exit
            pthread_exit(NULL);
        }

        char cwd[1000];

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
        }
        else {
            perror(RED "getcwd() error" RESET);
            exit(0);
        }
        strcat(cwd, file_path);

        // printf("***%s\n",file_buffer);

        struct stat fileStat;
        if (stat(cwd, &fileStat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        // Check if it is a directory
        char succ_mess[100];
        if (S_ISDIR(fileStat.st_mode)) {
            strcpy(succ_mess, DIR_READ);
            printf(RED "%s\n" RESET, DIR_READ);
            if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
                printf(RED "[-] Error sending data\n" RESET);
                exit(0);
            }
            return NULL;
        }

        FILE* file;

        // char buffer[1024];
        //  char succ_mess[100];
        file = fopen(cwd, "r");
        if (file == NULL) {
            perror(RED "[-] File opening error");
            if (send(client_sock, "failed", sizeof("failed"), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                exit(0);
            }

            return NULL;
        }
        strcpy(succ_mess, "success");
        if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
            perror(RED "[-] Error sending data" RESET);
            exit(0);
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            //  printf("**%s\n",buffer);
            if (send(client_sock, buffer, sizeof(buffer), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                exit(0);
            }
        }
        // snprintf(buffer, sizeof(buffer), "DONE");
        strcpy(buffer, "DONE");
        if (send(client_sock, buffer, sizeof(buffer), 0) == -1) {
            perror(RED "[-] Error sending data" RESET);
            exit(0);
        }

        fclose(file);
        sem_post(&sem_array[uniqueNumber]);
    }
    else if (strcmp(command, "7") == 0)// Permissions
    {
        if ((client_sock = accept(sock_ss_client, (struct sockaddr*)&cli_addr, &cli_addr_size))
            == -1) {
            perror(RED "[-] Accept error" RESET);
            exit(0);
        }

        char file_path[100];
        // Getting file path from client
        if ((received = recv(client_sock, file_path, sizeof(file_path), 0)) == -1) {
            printf(RED "[-] Error receiving data\n" RESET);
            exit(0);
        }

        // map the file path to a unique number
        unsigned int uniqueNumber = mapToRange(file_path);

        // Check if the file is already open
        if (sem_trywait(&sem_array[uniqueNumber]) == -1) {
            // The file is already open
            perror(RED "[-] The file is already open" RESET);
            // send error message to client
            char err_mess[100];
            strcpy(err_mess, "Resource busy");
            if (send(client_sock, err_mess, sizeof(err_mess), 0) == -1) {
                perror(RED "[-] Error sending data" RESET);
                // exit the thread
                pthread_exit(NULL);
            }
            pthread_exit(NULL);
        }

        char cwd[1000];

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
        }
        else {
            perror(RED "getcwd() error" RESET);
            exit(0);
        }
        strcat(cwd, file_path);

        // printf("***%s\n",file_buffer);

        struct stat fileStat;
        if (stat(cwd, &fileStat) == -1) {
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        // Check if it is a directory
        char succ_mess[100];
        if (S_ISDIR(fileStat.st_mode)) {
            strcpy(succ_mess, DIR_PERM);
            printf(RED "%s\n" RESET, DIR_PERM);
            if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
                printf(RED "[-] Error sending data\n" RESET);
                exit(0);
            }
            return NULL;
        }

        // char succ_mess[100];
        strcpy(succ_mess, "success");
        if (send(client_sock, succ_mess, sizeof(succ_mess), 0) == -1) {
            printf(RED "[-] Error sending data\n" RESET);
            exit(0);
        }
        // Getting file size

        char buffer[1024];
        if (stat(cwd, &fileStat) == -1) {
            perror(RED "Error getting file information" RESET);
            exit(0);
        }

        {
            // Storing file size in buffer
            snprintf(buffer,
                     sizeof(buffer),
                     "\nFile Size: %lld bytes\n\n",
                     (long long)fileStat.st_size);

            // Owner permissions
            if (fileStat.st_mode & S_IRUSR) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner has read permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner does not have read permission\n");
            }
            if (fileStat.st_mode & S_IWUSR) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner has write permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner does not have write permission\n");
            }
            if (fileStat.st_mode & S_IXUSR) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner has execute permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Owner does not have execute permission\n");
            }

            // Group permissions
            if (fileStat.st_mode & S_IRGRP) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group has read permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group does not have read permission\n");
            }
            if (fileStat.st_mode & S_IWGRP) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group has write permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group does not have write permission\n");
            }
            if (fileStat.st_mode & S_IXGRP) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group has execute permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Group does not have execute permission\n");
            }

            // Others permissions
            if (fileStat.st_mode & S_IROTH) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other has read permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other does not have read permission\n");
            }
            if (fileStat.st_mode & S_IWOTH) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other has write permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other does not have write permission\n");
            }
            if (fileStat.st_mode & S_IXOTH) {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other has execute permission\n");
            }
            else {
                snprintf(buffer + strlen(buffer),
                         sizeof(buffer) - strlen(buffer),
                         "Other does not have execute permission\n\n");
            }

            if (send(client_sock, buffer, sizeof(buffer), 0) == -1) {
                printf(RED "[-] Error sending data\n" RESET);
                exit(0);
            }

            printf(GREEN "Permissions Sent Successfully!\n" RESET);
            sem_post(&sem_array[uniqueNumber]);
        }
        // else
        // {
        //     perror(RED "[-] stat" RESET);
        //     if (send(client_sock, "failed", sizeof("failed"), 0) == -1)
        //     {
        //         printf(RED "[-] Error sending data\n" RESET);
        //         exit(0);
        //     }
        // }
    }
    // if 9 just send the string of paths_file
    else if(strcmp(command,"9")==0) {
        send(naming_server_sock, paths_file, sizeof(paths_file), 0);
    }
    // thread exit
    pthread_exit(NULL);
    
}

int main()
{
    // sem init
    for (int i = 0; i < MAX_NUM_FILES; ++i) {
        sem_init(&sem_array[i], 0, 1);
    }

    // REVERT BACK TO THIS
    //     printf("Enter the port number for client: ");
    //       scanf("%d", &port_for_client);
    //    // port_for_client = 5567;
    //     printf("Enter the port number for NS: ");
    //       scanf("%d", &port_for_nm);
    //    // port_for_nm = 5568;
    //     printf("Enter the IP address of NS: ");
    //       scanf("%s", ip);
    //  strcpy(ip, "127.0.0.1");

    while (1) {
        printf("Enter the port number for client: ");
        scanf("%d", &port_for_client);

        // as 5566 is reserved for naming server
        if (isPortAvailable(port_for_client) && port_for_client != 5566) {
            break;
        }
        else {
            // Port is not available
            printf(RED "Port %d is not available. Try a different port.\n" RESET, port_for_client);
        }
    }

    while (1) {
        printf("Enter the port number for NS: ");
        scanf("%d", &port_for_nm);

        if (isPortAvailable(port_for_nm) && port_for_nm != port_for_client) {
            break;
        }
        else {
            // Port is not available
            printf(RED "Port %d is not available. Try a different port.\n" RESET, port_for_nm);
        }
    }

    while (1) {
        printf("Enter name of paths file: ");
        scanf("%s", paths_file);

        if (access(paths_file, F_OK) == -1) {
            // File doesn't exist
            printf(RED "[-] File opening error: " RESET);
            printf("File '%s' doesn't exist.\n", paths_file);
        }
        else {
            printf(CYAN "[+] Trying to connect to NS...\n" RESET);
            break;
        }
    }
    //  strcpy(paths_file, "paths.txt");
    printf("\n");

    // HARDCODED INFO for testing
    // port_for_client = 5567;
    // port_for_nm     = 5568;

    if (strcmp(ip, "localhost")) {
        strcpy(ip, "127.0.0.1");
    }

    int sock, naming_server_sock;
    struct sockaddr_in addr, client_addr, ss_addr;
    socklen_t addr_size;
    int n;

    connect_to_naming_server(ip, &naming_server_sock, &ss_addr);
    int role = 1;
    if (send(naming_server_sock, &role, sizeof(role), 0) == -1) {
        perror(RED "[-]Error sending data" RESET);
        exit(1);
    }
    MakeSSsend_vital(&naming_server_sock, ip, &port_for_client, &port_for_nm, paths_file);
    printf("Sent vital details to NS\n");
    close_socket(&naming_server_sock);

    addr_size = sizeof(client_addr);

    // Defining variables for sending and recieving
    char file_path[MAX_FILE_PATH];
    char file_path_dest[MAX_FILE_PATH];
    char command[2];
    int received;
    int i = 0;

    int sock_ss_nm, sock_ss_client;
    struct sockaddr_in client_addr1, nm_addr;
    socklen_t client1_addr_size = sizeof(client_addr1);
    socklen_t nm_addr_size      = sizeof(nm_addr);
    init_port_create_sock(&sock_ss_client, &client_addr1, ip, port_for_client);
    init_port_create_sock(&sock_ss_nm, &nm_addr, ip, port_for_nm);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_size = sizeof(cli_addr);
    int client_sock;

    while (1) {
        naming_server_sock = accept(sock_ss_nm, (struct sockaddr*)&nm_addr, &nm_addr_size);
        if (naming_server_sock == -1) {
            perror("[-] Accept error");
            exit(0);
        }

        if ((received = recv(naming_server_sock, &command, sizeof(command), 0)) == -1) {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else if (received == 0) {
            // The client has closed the connection, so break out of the loop
            printf("Naming Server disconnected.\n");
            close(naming_server_sock);
            break;
        }
        else {
            command[received] = '\0';
        }

        // creating thread for each client
        pthread_t thread_id;
        struct ss_thread_args* args = (struct ss_thread_args*)malloc(sizeof(struct ss_thread_args));
        args->client_sock           = client_sock;
        args->cli_addr              = cli_addr;
        args->cli_addr_size         = cli_addr_size;
        args->naming_server_sock    = naming_server_sock;
        args->sock_ss_client        = sock_ss_client;
        args->received              = received;
        strcpy(args->command, command);
        pthread_create(&thread_id, NULL, handleClient, (void*)args);
        // detach the thread
        pthread_detach(thread_id);
    }
    close_socket(&naming_server_sock);
    close_socket(&sock_ss_client);
    close_socket(&client_sock);
    return 0;
}