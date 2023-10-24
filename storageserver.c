#include "header.h"

int main()
{
    int sock, client_sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;
    char *ip = "127.0.0.1";

    // Connect to SS
    connect_to_NS_from_SS(&sock, &addr, "127.0.0.1", 5566);

    addr_size = sizeof(client_addr);

    // Defining variables for sending and recieving
    char file_path[MAX_FILE_PATH];
    char command[2];
    int received;
    int i = 0;
    while (1)
    {
        client_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock == -1)
        {
            perror("[-] Accept error");
            exit(0);
        }

        if ((received = recv(client_sock, &command, sizeof(command), 0)) == -1)
        {
            printf("Error recieving data\n");
            exit(0);
        }
        else if (received == 0)
        {
            // The client has closed the connection, so break out of the loop
            printf("Client disconnected.\n");
            close(client_sock);
            break;
        }
        else
        {
            command[received] = '\0';
        }

        if (strcmp(command, "1") == 0)
        {
        }
        else if (strcmp(command, "2") == 0)
        {
            if ((received = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                file_path[received] = '\0';
            }

            char *temp = (char *)malloc(sizeof(char) * 1000);
            strcpy(temp, file_path);

            char option[10];

            if ((received = recv(client_sock, &option, sizeof(option), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                option[received] = '\0';
            }

            // If option is 1, delete a file, if option is 2, delete a directory
            if (strcmp(option, "1") == 0)
            {
                delete_file(temp);
            }
            else if (strcmp(option, "2") == 0)
            {
                delete_directory(temp);
            }
            // Sending success message
            int sent = send(client_sock, "done", sizeof("done"), 0);
            if (sent == -1)
            {
                perror("Error sending data");
            }
        }
        else if (strcmp(command, "3") == 0)
        {
            if ((received = recv(client_sock, &file_path, sizeof(file_path), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                file_path[received] = '\0';
            }

            char *temp = (char *)malloc(sizeof(char) * 1000);
            strcpy(temp, file_path);

            char option[10];

            if ((received = recv(client_sock, &option, sizeof(option), 0)) == -1)
            {
                printf("Error recieving data\n");
                exit(0);
            }
            else
            {
                option[received] = '\0';
            }

            // // If option is 1, create a file, if option is 2, create a directory
            if (strcmp(option, "1") == 0)
            {
                create_file(temp);
            }
            else if (strcmp(option, "2") == 0)
            {
                create_directory(temp);
            }
            // Sending success message
            int sent = send(client_sock, "done", sizeof("done"), 0);
            if (sent == -1)
            {
                perror("Error sending data");
            }
        }
        else if (strcmp(command, "5") == 0)
        {
            // Forming connection with client
            int cli_temp_sock;
            struct sockaddr_in cli_temp_addr;
            connect_to_client(&cli_temp_sock, &cli_temp_addr, "127.0.0.1", 5568);

            struct sockaddr_in cli_addr;
            socklen_t cli_addr_size = sizeof(cli_addr);
            int cli_sock;

            if ((cli_sock = accept(cli_temp_sock, (struct sockaddr *)&cli_addr, &cli_addr_size)) == -1)
            {
                perror("[-] Accept error");
                exit(0);
            }
            close_socket(&cli_temp_sock);
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

                if (valid_data && strlen(received_data_to_write)>=1)
                {
                    fprintf(file, "%s\n", received_data_to_write);
                }
            }

            fclose(file);

            close_socket(&cli_sock);
        }
        else if (strcmp(command, "6") == 0)
        {
            // Forming connection with client
            int cli_temp_sock;
            struct sockaddr_in cli_temp_addr;
            connect_to_client(&cli_temp_sock, &cli_temp_addr, "127.0.0.1", 5568);

            struct sockaddr_in cli_addr;
            socklen_t cli_addr_size = sizeof(cli_addr);
            int cli_sock;

            if ((cli_sock = accept(cli_temp_sock, (struct sockaddr *)&cli_addr, &cli_addr_size)) == -1)
            {
                perror(RED "[-] Accept error" RESET);
                exit(0);
            }
            close_socket(&cli_temp_sock);
            char file_path[100];
            // Getting file path from client
            if ((received = recv(cli_sock, file_path, sizeof(file_path), 0)) == -1)
            {
                printf(RED "Error receiving data\n" RESET);
                exit(0);
            }

            FILE *file;
            char buffer[1024];

            file = fopen(file_path, "r");
            if (file == NULL)
            {
                perror(RED "[-] File opening error" RESET);
                return 1;
            }
            int line_count = 0;
            // Counting number of lines in the file
            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                line_count++;
            }
            char line_count_str[10];
            snprintf(line_count_str, sizeof(line_count_str), "%d", line_count);
            // Sending number of lines in the file
            if (send(cli_sock, line_count_str, sizeof(line_count_str), 0) == -1)
            {
                printf(RED "Error sending data\n" RESET);
                exit(0);
            }
            rewind(file);
            // Sending file contents line by line

            printf("Sending file contents...\n");

            while (fgets(buffer, sizeof(buffer), file) != NULL)
            {
                if (send(cli_sock, buffer, sizeof(buffer), 0) == -1)
                {
                    perror(RED "[-] Error sending data\n" RESET);
                    exit(0);
                }
            }

            fclose(file);

            close_socket(&cli_sock);
        }
    }
    close(client_sock);
    return 0;
}
