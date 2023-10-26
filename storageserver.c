#include "header.h"

int main()
{
    int port_for_client;
    int port_for_nm;
    char ip[16] = {'\0'};
    char paths_file[MAX_FILE_NAME] = {'\0'};

    printf("Enter the port number for client: ");
    scanf("%d", &port_for_client);
    printf("Enter the port number for NS: ");
    scanf("%d", &port_for_nm);
    printf("Enter the IP address of NS: ");
    scanf("%s", ip);
    printf("Enter name of paths file: ");
    scanf("%s", paths_file);

    FILE *file = fopen(paths_file, "r");
    if (file == NULL)
    {
        perror(RED "[-] File opening error" RESET);
        return 1;
    }
    char buffer[MAX_NUM_PATHS] = {'\0'};

    size_t len;
    if (fread(buffer, sizeof(buffer), 1, file) == -1)
    {
        perror(RED "[-] File reading error" RESET);
        return 1;
    }

    int sock, naming_server_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_size;
    int n;

    // Connect to SS
    connect_to_NS_from_SS(&sock, &addr, ip, 5566);

    addr_size = sizeof(client_addr);

    // Defining variables for sending and recieving
    char file_path[MAX_FILE_PATH];
    char command[2];
    int received;
    int i = 0;

    // Forming connection with client
    int client_sock;
    struct sockaddr_in cli_temp_addr;
    connect_to_client(&client_sock, &cli_temp_addr, "127.0.0.1", port_for_client);

    struct sockaddr_in cli_addr;
    socklen_t cli_addr_size = sizeof(cli_addr);
    int cli_sock;

    while (1)
    {
        naming_server_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_size);
        if (naming_server_sock == -1)
        {
            perror("[-] Accept error");
            exit(0);
        }

        if ((received = recv(naming_server_sock, &command, sizeof(command), 0)) == -1)
        {
            printf(RED "Error recieving data\n" RESET);
            exit(0);
        }
        else if (received == 0)
        {
            // The client has closed the connection, so break out of the loop
            printf("Client disconnected.\n");
            close(naming_server_sock);
            break;
        }
        else
        {
            command[received] = '\0';
        }

        if (strcmp(command, "1") == 0) // Init
        {
            // printf("File contents:\n%s\n", buffer);
            if (send(naming_server_sock, buffer, sizeof(buffer), 0) == -1)
                perror(RED "[-] Error sending data" RESET);
            if (send(naming_server_sock, &port_for_client, sizeof(port_for_client), 0) == -1)
                perror(RED "[-] Error sending data" RESET);
            if (send(naming_server_sock, &port_for_nm, sizeof(port_for_nm), 0) == -1)
                perror(RED "[-] Error sending data" RESET);
            if (send(naming_server_sock, ip, sizeof(ip), 0) == -1)
                perror(RED "[-] Error sending data" RESET);
        }
        else if (strcmp(command, "2") == 0) // Deletion
        {
            if ((received = recv(naming_server_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                printf(RED "Error recieving data\n" RESET);
                exit(0);
            }
            else
            {
                file_path[received] = '\0';
            }

            char *temp = (char *)malloc(sizeof(char) * 1000);
            strcpy(temp, file_path);

            char option[10];

            if ((received = recv(naming_server_sock, &option, sizeof(option), 0)) == -1)
            {
                printf(RED "Error recieving data\n" RESET);
                exit(0);
            }
            else
            {
                option[received] = '\0';
            }

            int del = 0;
            // If option is 1, delete a file, if option is 2, delete a directory
            if (strcmp(option, "1") == 0)
            {
                if (delete_file(temp) == -1)
                    del = 1;
            }
            else if (strcmp(option, "2") == 0)
            {
                if (delete_directory(temp) == -1)
                    del = 1;
            }

            if (Delete_from_path_file(file_path, paths_file) == 0 && del == 0)
            {
                printf(GREEN "Deleted Successfully!\n" RESET);
                int sent = send(naming_server_sock, "done", sizeof("done"), 0);
                if (sent == -1)
                {
                    perror(RED "[-] Error sending data" RESET);
                }
            }
            else
            {
                perror(RED "[-] Error deleting file/directory" RESET);
                int sent = send(naming_server_sock, "not done", sizeof("not done"), 0);
                if (sent == -1)
                {
                    perror(RED "[-] Error sending data" RESET);
                }
            }
        }
        else if (strcmp(command, "3") == 0) // Creation
        {
            if ((received = recv(naming_server_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                printf(RED "Error recieving data\n" RESET);
                exit(0);
            }
            else
            {
                file_path[received] = '\0';
            }

            char *temp = (char *)malloc(sizeof(char) * 1000);
            strcpy(temp, file_path);

            char option[10];

            if ((received = recv(naming_server_sock, &option, sizeof(option), 0)) == -1)
            {
                printf(RED "Error recieving data\n" RESET);
                exit(0);
            }
            else
            {
                option[received] = '\0';
            }

            // If option is 1, create a file, if option is 2, create a directory
            if (strcmp(option, "1") == 0)
            {
                if (create_file(temp) == -1)
                {
                    int sent = send(naming_server_sock, "Creation Error!!", sizeof("Creation Error!!"), 0);
                    if (sent == -1)
                    {
                        perror(RED "Error sending data" RESET);
                    }
                    continue;
                }
            }
            else if (strcmp(option, "2") == 0)
            {
                if (create_directory(temp) == -1)
                {
                    int sent = send(naming_server_sock, "Creation Error!!", sizeof("Creation Error!!"), 0);
                    if (sent == -1)
                    {
                        perror("Error sending data");
                    }
                    continue;
                }
            }
            if (Add_to_path_file(file_path, paths_file) == 0)
                printf(GREEN "Created Successfully!\n" RESET);
            else
                perror(RED "[-] Error creating file/directory" RESET);

            // Sending success message
            int sent = send(naming_server_sock, "done", sizeof("done"), 0);
            if (sent == -1)
            {
                perror(RED "Error sending data" RESET);
            }
        }
        else if (strcmp(command, "5") == 0) // Writing
        {
            if ((cli_sock = accept(client_sock, (struct sockaddr *)&cli_addr, &cli_addr_size)) == -1)
            {
                perror("[-] Accept error");
                exit(0);
            }
            // Forming connection with client

            char file_path[100];
            // Getting file path from client
            if ((received = recv(cli_sock, file_path, sizeof(file_path), 0)) == -1)
            {
                printf("Error receiving data\n");
                exit(0);
            }

            FILE *file;

            // Open the file for writing
            if ((file = fopen(file_path, "w")) == NULL)
            {
                perror("[-] Could not open the file");
                return 1;
            }
            int received_to_write = 0;
            char received_data_to_write[1024];

            while (1)
            {
                received_to_write = recv(cli_sock, received_data_to_write, sizeof(received_data_to_write), 0);

                if (received_to_write == -1)
                {
                    perror("[-] Receive error");
                    break;
                }

                if (received_to_write == 0)
                {
                    // The client closed the connection
                    break;
                }

                // Check if the received data indicates the end of the transfer
                if (strcmp(received_data_to_write, "done") == 0 || strcmp(received_data_to_write, "done\n") == 0)
                {
                    break;
                }

                // Check if the received data is valid before writing to the file
                int valid_data = 1;
                for (size_t i = 0; i < strlen(received_data_to_write); i++)
                {
                    if (!isprint(received_data_to_write[i]))
                    {
                        valid_data = 0;
                        break;
                    }
                }

                if (valid_data && strlen(received_data_to_write) >= 1)
                {
                    fprintf(file, "%s\n", received_data_to_write);
                }
            }

            fclose(file);

            close_socket(&cli_sock);
        }
        else if (strcmp(command, "6") == 0) // Reading
        {
            if ((cli_sock = accept(client_sock, (struct sockaddr *)&cli_addr, &cli_addr_size)) == -1)
            {
                perror("[-] Accept error");
                exit(0);
            }

            char file_path[100];
            // Getting file path from client
            if ((received = recv(cli_sock, file_path, sizeof(file_path), 0)) == -1)
            {
                printf("Error receiving data\n");
                exit(0);
            }

            FILE *file;
            char buffer[1024];

            file = fopen(file_path, "r");
            if (file == NULL)
            {
                perror("[-] File opening error");
                return 1;
            }
            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                send(cli_sock, buffer, sizeof(buffer), 0);
            }
            snprintf(buffer, sizeof(buffer), "DONE");
            send(cli_sock, buffer, sizeof(buffer), 0);

            fclose(file);
        }
        else if (strcmp(command, "7") == 0) // Permissions
        {
            if ((cli_sock = accept(client_sock, (struct sockaddr *)&cli_addr, &cli_addr_size)) == -1)
            {
                perror(RED "[-] Accept error" RESET);
                exit(0);
            }

            char file_path[100];
            // Getting file path from client
            if ((received = recv(cli_sock, file_path, sizeof(file_path), 0)) == -1)
            {
                printf(RED "[-] Error receiving data\n" RESET);
                exit(0);
            }
            if (strcmp(file_path, "failed") == 0)
            {
                printf(RED "[-] File does not exist\n" RESET);
                continue;
            }
            struct stat fileStat;

            // Getting file size
            off_t fileSize = fileStat.st_size;
            if (fileSize == 0)
            {
                printf(RED "[-] File does not exist\n" RESET);
                if (send(cli_sock, "failed", sizeof("failed"), 0) == -1)
                {
                    printf(RED "[-] Error sending data\n" RESET);
                    exit(0);
                }

                continue;
            }

            char buffer[1024];

            if (stat(file_path, &fileStat) == 0)
            {
                // Storing file size in buffer
                snprintf(buffer, sizeof(buffer), "File Size: %lld bytes\n", (long long)fileStat.st_size);

                // Owner permissions
                if (fileStat.st_mode & S_IRUSR)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner has read permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner does not have read permission\n");
                if (fileStat.st_mode & S_IWUSR)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner has write permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner does not have write permission\n");
                if (fileStat.st_mode & S_IXUSR)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner has execute permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Owner does not have execute permission\n");

                // Group permissions
                if (fileStat.st_mode & S_IRGRP)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group has read permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group does not have read permission\n");
                if (fileStat.st_mode & S_IWGRP)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group has write permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group does not have write permission\n");
                if (fileStat.st_mode & S_IXGRP)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group has execute permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Group does not have execute permission\n");

                // Others permissions
                if (fileStat.st_mode & S_IROTH)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other has read permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other does not have read permission\n");
                if (fileStat.st_mode & S_IWOTH)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other has write permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other does not have write permission\n");
                if (fileStat.st_mode & S_IXOTH)
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other has execute permission\n");
                else
                    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "Other does not have execute permission\n");

                if (send(cli_sock, buffer, sizeof(buffer), 0) == -1)
                {
                    printf(RED "[-] Error sending data\n" RESET);
                    exit(0);
                }

                printf("Permissions Sent Successfully!\n");
            }
            else
            {
                perror(RED "[-] stat" RESET);
                if (send(cli_sock, "failed", sizeof("failed"), 0) == -1)
                {
                    printf(RED "[-] Error sending data\n" RESET);
                    exit(0);
                }
            }
        }
    }
    close_socket(&client_sock);
    close_socket(&cli_sock);
    close(naming_server_sock);
    return 0;
}