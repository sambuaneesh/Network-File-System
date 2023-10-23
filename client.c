#include "header.h"

int main() {
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_size;

    // Create a socket and connect to the naming server
    connect_to_naming_server("127.0.0.1", &client_sock, &client_addr);

    while (1) {
        int option = -1;
        printf("\nDo you want to:\n");
        printf("Connect to a storage server     (enter 1)\n");
        printf("Create an Empty File/Directory  (enter 2)\n");
        printf("Delete a File/Directory         (enter 3)\n");
        printf("Copy Files/Directories          (enter 4)\n");
        printf("Edit File                       (enter 5)\n");
        printf("Read File                       (enter 6)\n");
        printf("Write To File                   (enter 7)\n");
        printf("Get File Info                   (enter 8)\n");

        scanf("%d", &option);

        if (option == 1) {
            if (send(client_sock, "1", strlen("1"), 0) == -1) {
                perror("[-]Send error");
                exit(1);
            }
        }
        else if (option == 2) {
            if (send(client_sock, "2", strlen("2"), 0) == -1) {
                perror("[-]Send error");
                exit(1);
            }
        }
        else if (option == 3) {
            
            if (send(client_sock, "3", strlen("3"), 0) == -1) {
                perror("[-]Send error");
                exit(1);
            }

            char storage_server_num[10];
           int  num_storage_servers = 10;
            printf("Enter the storage server number: ");
            scanf("%s", storage_server_num);
           
 if (atoi(storage_server_num) > num_storage_servers)
            {
                printf("Invalid storage server number\n");
                continue;
            }
            else{
                if (send(client_sock, storage_server_num, sizeof(storage_server_num), 0) == -1) {
                perror("[-]Send error");
                exit(1);
            }
            }

            char path[MAX_FILE_PATH];
            printf("Enter the path: ");
            scanf("%s", path);


            

            if (send(client_sock, path, sizeof(path), 0) == -1)
                printf("[-]Send error\n");
                
            char create_option[10];

            printf("Do you want to\n");
            printf("Create an empty file       (Enter 1)\n");
            printf("Create an empty directory  (Enter 2)\n");
            scanf("%s", create_option);



            if (send(client_sock, create_option, sizeof(create_option), 0) == -1)
                printf("[-]Send error\n");
            
            printf(" ");
       
        }
         else {
            printf("Invalid option\n");
        }
    }

    // Close the client socket when done
    close(client_sock);

    return 0;
}
