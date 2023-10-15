#include "header.h"

int main()
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    open_naming_server_port(5566, &server_sock, &server_addr);
    // we now have a dedicated port for the naming server

    storage_servers list = NULL;
    int num_storage_servers = 0;

    while (1)
    {
        int option = -1;
        printf("\nDo you want to\n");
        printf("Connect to a storage server     (enter 1)\n");
        printf("Connect to a client             (enter 2)\n");
        printf("Create an Empty File/Directory  (enter 3)\n");
        printf("Delete a File/Directory         (enter 4)\n");
        printf("Copy Files/Directories          (enter 5)\n");
        scanf("%d", &option);
        if (option == 1) // connect to storage server
        {
            listen_for_client(&server_sock, &client_sock, &client_addr, &addr_size);

            ss_send *received_data = (ss_send *)malloc(sizeof(ss_send));
            if (recv(client_sock, received_data, sizeof(*received_data), 0) == -1)
                printf("[-]Receive error");

            // add to list of storage servers
            // need to iterate through all of them to check if it is already there for reconnection
            // need to modify in aplhabetical order or something similar for faster search
            storage_servers new = (storage_servers)malloc(sizeof(ss));
            new->next = list;
            new->ss_send = received_data;
            new->num_server = num_storage_servers++;
            list = new;

            close_socket(&server_sock);
            open_naming_server_port(received_data->server_port, &server_sock, &server_addr);
            // we now have a dedicated port for the storage server
            listen_for_client(&server_sock, &client_sock, &client_addr, &addr_size);
            // we now have a dedicated socket for the storage server
            close_socket(&received_data->server_port);
            open_naming_server_port(5566, &server_sock, &server_addr);
        }
        else if (option == 2) // client server
        {
            if (listen(server_sock, 5) == -1)
            {
                perror("[-]Listen error");
                exit(1);
            }

            printf("Listening...\n");
            addr_size = sizeof(client_addr);
            client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
            printf("[+]Client connected.\n");
        }
        else if (option == 3) // create file/directory
        {
            int storage_server_num = -1;
            printf("Enter the storage server number: ");
            scanf("%d", &storage_server_num);
            if (storage_server_num >= num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            char path[MAX_PATH_SIZE];
            printf("Enter the path: ");
            scanf("%s", path);

            // send to storage server
            if (send(client_sock, "create", sizeof("create"), 0) == -1)
                printf("[-]Send error\n");
            if (send(client_sock, path, strlen(path), 0) == -1)
                printf("[-]Send error\n");

            ss *temp = list;
            while (temp->num_server != storage_server_num)
                temp = temp->next;

            paths *new = (paths *)malloc(sizeof(paths));
            strcpy(new->path, path);
            // add to linked list of paths just at the beginning
            new->next = temp->ss_send->files_and_dirs;
        }
        else if (option == 4) // delete a file/directory
        {
            int storage_server_num = -1;
            printf("Enter the storage server number: ");
            scanf("%d", &storage_server_num);
            if (storage_server_num >= num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            char path[MAX_PATH_SIZE];
            printf("Enter the path: ");
            scanf("%s", path);

            // send to storage server
            if (send(client_sock, "delete", sizeof("create"), 0) == -1)
                printf("[-]Send error\n");
            if (send(client_sock, path, strlen(path), 0) == -1)
                printf("[-]Send error\n");

            ss *temp = list;
            while (temp->num_server != storage_server_num)
                temp = temp->next;

            paths *prev = NULL;
            paths *curr = temp->ss_send->files_and_dirs;
            while (curr != NULL)
            {
                if (strcmp(curr->path, path) == 0)
                {
                    if (prev == NULL)
                        temp->ss_send->files_and_dirs = curr->next;
                    else
                        prev->next = curr->next;
                    free(curr);
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
        }
        else if (option == 5) // copy files/directories
        {
            int storage_server_num_copy = -1;
            printf("Enter the storage server number to copy from: ");
            scanf("%d", &storage_server_num_copy);
            if (storage_server_num_copy >= num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            int storage_server_num_paste = -1;
            printf("Enter the storage server number to copy from: ");
            scanf("%d", &storage_server_num_paste);
            if (storage_server_num_paste >= num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }

            char path1[MAX_PATH_SIZE];
            printf("Enter the path of the file/directory to be copied: ");
            scanf("%s", path1);

            char path2[MAX_PATH_SIZE];
            printf("Enter the path of the destination file/directory: ");
            scanf("%s", path2);

            // send to storage server
            if (send(client_sock, "copy", sizeof("copy"), 0) == -1)
                printf("[-]Send error\n");
            if (send(client_sock, path1, strlen(path1), 0) == -1)
                printf("[-]Send error\n");

            if (send(client_sock, path2, strlen(path2), 0) == -1)
                printf("[-]Send error\n");

            ss *temp = list;
            while (temp->num_server != storage_server_num_copy)
                temp = temp->next;

            paths *prev = NULL;
            paths *curr = temp->ss_send->files_and_dirs;
            while (curr != NULL)
            {
                if (strcmp(curr->path, path1) == 0)
                {
                    if (prev == NULL)
                        temp->ss_send->files_and_dirs = curr->next;
                    else
                        prev->next = curr->next;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
        }
        else
            printf("Invalid option\n");
    }
    close_socket(&server_sock);
    return 0;
}