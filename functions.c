#include "header.h"
storage_servers storage_server_list;

// Tree and LL functions
Tree MakeNode(char *name)
{
    Tree T = (Tree)malloc(sizeof(struct TreeNode));
    strcpy(T->path, name);
    T->first_child = NULL;
    T->next_sibling = NULL;
    T->prev_sibling = NULL;
    T->parent = NULL;

    return T;
}

Tree Insert(Tree parent, char *path)
{
    Tree traveller = parent->first_child;

    while (traveller != NULL && traveller->next_sibling != NULL)
        traveller = traveller->next_sibling;

    char *token = strtok_r(path, "/", &path);
    while (token != NULL)
    {
        Tree new = MakeNode(token);
        if (traveller == NULL)
            parent->first_child = new;
        else
        {
            traveller->next_sibling = new;
            new->prev_sibling = traveller;
            traveller = NULL;
        }
        new->parent = parent;
        parent = new;
        token = strtok_r(path, "/", &path);
    }

    return parent;
}

Tree Search_Till_Parent(Tree T, char *path, int insert)
{
    Tree parent = T;
    Tree traveller = parent->first_child;

    char so_far[MAX_FILE_PATH] = {'\0'};

    char *path_duplicate = (char *)malloc(MAX_FILE_PATH * sizeof(char));
    char *path_duplicate2 = (char *)malloc(MAX_FILE_PATH * sizeof(char));
    for (int i = 0; i < MAX_FILE_PATH; i++)
    {
        path_duplicate[i] = '\0';
        path_duplicate2[i] = '\0';
    }
    strcpy(path_duplicate, path);
    strcpy(path_duplicate2, path);

    char *token = strtok_r(path_duplicate, "/", &path_duplicate);
    while (token != NULL)
    {
        while (traveller != NULL)
        {
            if (strcmp(traveller->path, token) == 0)
                break;
            traveller = traveller->next_sibling;
        }
        if (traveller == NULL) // if we reach the end of linked list and do not reach a match
        {
            // break the string and send remaining part to function
            // send parent also to function
            int count = 0;
            for (int i = strlen(so_far) + 1; i < strlen(path_duplicate2); i++)
            {
                if (path_duplicate2[i] == '/')
                    count++;
            }
            if (count >= 1)
            {
                printf(RED "Path not found\n" RESET);
                return NULL;
            }
            if (insert == 1)
            {
                parent = Insert(parent, path_duplicate2 + strlen(so_far));
                return T;
            }
            else
                return NULL;
        }
        else
        {
            parent = traveller;
            traveller = traveller->first_child;
        }
        strcat(so_far, token);
        strcat(so_far, "/");
        token = strtok_r(NULL, "/", &path_duplicate);
    }

    return parent;
}

void PrintTree(Tree T)
{
    if (T == NULL)
        return;

    printf("%s\n", T->path);
    PrintTree(T->first_child);
    PrintTree(T->next_sibling);
}

void Del_Rec(Tree T)
{
    if (T == NULL)
        return;

    Del_Rec(T->first_child);
    Del_Rec(T->next_sibling);
    free(T);
}

int Delete_Path(Tree T, char *path)
{
    if (T == NULL)
        return -1;
    Tree traveller = Search_Till_Parent(T, path, 0);
    if (traveller == NULL)
    {
        printf(RED "Path not found\n" RESET);
        return -1;
    }
    if (traveller->parent == NULL)
    {
        printf(RED "Cannot delete root\n" RESET);
        return -1;
    }
    if (traveller->parent->first_child == traveller)
        traveller->parent->first_child = traveller->next_sibling;
    if (traveller->prev_sibling != NULL)
        traveller->prev_sibling->next_sibling = traveller->next_sibling; // diconnected the dir now

    Del_Rec(traveller->first_child);
    free(traveller);

    return 0;
}

storage_servers check_if_path_in_ss(char *file_path, int insert) // NULL if not found else returns the parent depending on value of insert
{
    storage_servers traveller = storage_server_list;
    while (traveller != NULL)
    {
        Tree parent = Search_Till_Parent(traveller->files_and_dirs, file_path, insert);
        if (parent != NULL)
            return traveller;
        traveller = traveller->next;
    }
    return NULL;
}

storage_servers MakeNode_ss(char *ip_addr, int client_port, int server_port)
{
    storage_servers new = (storage_servers)malloc(sizeof(ss));
    new->ss_send = (ss_send *)malloc(sizeof(ss_send));
    strcpy(new->ss_send->ip_addr, ip_addr);
    new->ss_send->client_port = client_port;
    new->ss_send->server_port = server_port;
    new->files_and_dirs = MakeNode(".");
    new->next = NULL;

    return new;
}

void PrintAll()
{
    storage_servers traveller = storage_server_list;
    while (traveller != NULL)
    {
        printf("IP: %s\n", traveller->ss_send->ip_addr);
        printf("Client Port: %d\n", traveller->ss_send->client_port);
        printf("Server Port: %d\n", traveller->ss_send->server_port);
        printf("Files and Directories:\n");
        PrintTree(traveller->files_and_dirs);
        printf("\n");
        traveller = traveller->next;
    }
}

// Path file functions
int Add_to_path_file(char *file_path, char *storage_file)
{
    FILE *file = fopen(storage_file, "a");
    if (file == NULL)
    {
        perror("Error opening the file");
        return -1;
    }

    fprintf(file, "%s\n", file_path);
    fclose(file);
    return 0;
}

int Delete_from_path_file(char *file_path, char *storage_file)
{
    FILE *file = fopen(storage_file, "r");
    if (file == NULL)
    {
        perror("Error opening the file");
        return -1;
    }
    FILE *temp_file = fopen("dollar.txt", "w");
    if (temp_file == NULL)
    {
        perror("Error opening the file");
        return -1;
    }

    char buffer[MAX_FILE_PATH];
    char prefix_path[MAX_FILE_PATH] = {'\0'};
    strcpy(prefix_path, file_path);
    strcat(prefix_path, "/");

    printf("path: %s\n", file_path);

    while (fgets(buffer, MAX_FILE_PATH, file) != NULL)
    {
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';
        if (strcmp(buffer, file_path) != 0 && strncmp(buffer, prefix_path, strlen(prefix_path)) != 0)
        {
            fprintf(temp_file, "%s", buffer);
            fprintf(temp_file, "\n");
        }
    }

    fclose(file);
    fclose(temp_file);
    remove(storage_file);
    rename("dollar.txt", storage_file);

    return 0;
}

void load_SS(Tree T, char *file_name)
{
    char line[1024];
    FILE *file = fopen(file_name, "r");

    if (file == NULL)
    {
        perror("Error opening the file");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Remove the newline character (if it exists)
        size_t len = strcspn(line, "\n");
        if (line[len] == '\n')
        {
            line[len] = '\0'; // Replace newline with null-terminator
        }

        T = Search_Till_Parent(T, line, 1);
    }

    fclose(file);
}

// Creation and Deletion of files and folders
void get_path_details(char *path_to_go_to, char *file_name, char *file_path)
{
    // Extracting the index of the start of the file name - the part after the last '\'
    int i = 0;
    for (i = strlen(file_path) - 1; i >= 0; i--)
    {
        if (file_path[i] == '/')
        {
            break;
        }
    }
    int j;
    int ind = 0;

    // Getting the file name
    for (j = i + 1; j < strlen(file_path); j++)
    {
        file_name[ind] = file_path[j];
        ind++;
    }
    file_name[ind] = '\0';

    // Getting the path
    for (j = 0; j < i; j++)
    {
        path_to_go_to[j] = file_path[j];
    }
    path_to_go_to[j] = '\0';
}

int create_file(char *file_path)
{
    // Finding the directory we need to change to
    char *path_to_go_to = (char *)malloc(sizeof(char) * MAX_FILE_PATH);
    char *file_name = (char *)malloc(sizeof(char) * MAX_FILE_NAME);
    char current_dir[MAX_FILE_PATH];

    // Getting the current directory

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror(RED "[-]getcwd" RESET);
        return -1;
    }

    get_path_details(path_to_go_to, file_name, file_path);

    if (chdir(path_to_go_to) == -1)
    {
        perror(RED "[-]chdir" RESET);
        return -1;
    }

    // Creating a new file

    FILE *file = fopen(file_name, "r");
    if (file != NULL)
    {
        printf(RED "[-]File already exists\n" RESET);
        return -1;
    }
    file = fopen(file_name, "w");
    if (file == NULL)
    {
        perror(RED "[-]fopen" RESET);
        return -1;
    }
    else
    {
        // printf("File Created Successfully!\n");
    }
    fclose(file);

    if (chdir(current_dir) == -1)
    {
        perror(RED "[-]chdir" RESET);
        return -1;
    }

    return 0;
}

int create_directory(char *file_path)
{
    // Finding the directory we need to change to
    char *path_to_go_to = (char *)malloc(sizeof(char) * MAX_FILE_PATH);
    char *directory_name = (char *)malloc(sizeof(char) * MAX_FILE_NAME);
    char current_dir[MAX_FILE_PATH];

    // Getting the current directory

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror(RED "getcwd" RESET);
        return -1;
    }

    get_path_details(path_to_go_to, directory_name, file_path);

    if (chdir(path_to_go_to) == -1)
    {
        perror(RED "chdir" RESET);
        return -1;
    }

    // Creating the directory

    if (mkdir(directory_name, 0777) == 0)
    {
        // printf("Directory created successfully!\n");
    }
    else
    {
        perror(RED "mkdir" RESET);
        return -1;
    }

    if (chdir(current_dir) == -1)
    {
        perror(RED "chdir" RESET);
        return -1;
    }
    return 0;
}

int delete_file(char *file_path)
{
    // Finding the directory we need to change to
    char *path_to_go_to = (char *)malloc(sizeof(char) * MAX_FILE_PATH);
    char *file_name = (char *)malloc(sizeof(char) * MAX_FILE_NAME);
    char current_dir[MAX_FILE_PATH];

    // Getting the current directory

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror(RED "getcwd" RESET);
        return -1;
    }

    get_path_details(path_to_go_to, file_name, file_path);

    if (chdir(path_to_go_to) == -1)
    {
        perror(RED "chdir" RESET);
        return -1;
    }

    // Deleting the file
    if (remove(file_name) != 0)
    {
        perror(RED "remove" RESET);
        return -1;
    }

    if (chdir(current_dir) == -1)
    {
        perror(RED "chdir" RESET);
        return -1;
    }

    // printf("File Deleted Successfully!\n");
}

int delete_directory(char *file_path)
{
    // Finding the directory we need to change to
    char *path_to_go_to = (char *)malloc(sizeof(char) * MAX_FILE_PATH);
    char *directory_name = (char *)malloc(sizeof(char) * MAX_FILE_NAME);
    char current_dir[MAX_FILE_PATH];

    // Getting the current directory

    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror(RED "[-]getcwd" RESET);
        return -1;
    }

    get_path_details(path_to_go_to, directory_name, file_path);

    if (chdir(path_to_go_to) == -1)
    {
        perror(RED "chdir" RESET);
        return -1;
    }

    // Deleting the directory

    if (rmdir(directory_name) != 0)
    {
        if (delete_non_empty_dir(directory_name) == -1)
            return -1;
        // perror(RED "rmdir" RESET);
    }

    if (chdir(current_dir) == -1)
    {
        perror(RED "[-]chdir" RESET);
        return -1;
    }
    // printf("Directory Deleted Successfully!\n");
    return 0;
}

int delete_non_empty_dir(char *directory_name)
{
    DIR *dir;
    struct dirent *entry;
    dir = opendir(directory_name);
    if (!dir)
    {
        perror(RED "[-]opendir" RESET);
        return -1;
    }

    while ((entry = readdir(dir)))
    {
        if (entry->d_type != DT_DIR)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char path[MAX_FILE_PATH];
                snprintf(path, sizeof(path), "%s/%s", directory_name, entry->d_name);

                if (remove(path) != 0)
                {
                    perror(RED "remove" RESET);
                    closedir(dir);
                    return -1;
                }
            }
        }
        else
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char path[MAX_FILE_PATH];
                snprintf(path, sizeof(path), "%s/%s", directory_name, entry->d_name);
                delete_non_empty_dir(path);
            }
        }
    }

    closedir(dir);
    if (rmdir(directory_name) != 0)
    {
        perror(RED "rmdir" RESET);
        return -1;
    }
    return 0;
}

// socket functions
void close_socket(int *client_sock)
{
    if (close(*client_sock) == -1)
    {
        perror(RED "[-]Close error" RESET);
        exit(1);
    }
    else
        printf("[+]Client disconnected.\n\n");
}

void listen_for_client(int *server_sock, int *client_sock, struct sockaddr_in *client_addr, socklen_t *addr_size)
{
    if (listen(*server_sock, 5) == -1)
    {
        perror(RED "[-]Listen error" RESET);
        exit(1);
    }

    printf("Listening...\n");
    *addr_size = sizeof(*client_addr);
    *client_sock = accept(*server_sock, (struct sockaddr *)client_addr, addr_size);
    if (*client_sock == -1)
    {
        perror(RED "[-]Accept error" RESET);
        exit(1);
    }
    else
        printf("[+]Client connected.\n");
}

void connect_to_naming_server(char *ip, int *sock, struct sockaddr_in *addr)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0)
    {
        perror(RED "[-]Socket error" RESET);
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(addr, '\0', sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = port;
    addr->sin_addr.s_addr = inet_addr(ip); // converts the string to an acceptable form

    if (connect(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        printf(RED "[-]Connect error" RESET);
        exit(1);
    }
    else
        printf("[+]Connected to the naming server.\n");
}

void open_naming_server_port(int port_number, int *server_sock, struct sockaddr_in *server_addr)
{
    char *ip = "127.0.0.1";
    int n;

    *server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_sock < 0)
    {
        perror(RED "[-]Socket error" RESET);
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(server_addr, '\0', sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = port;
    server_addr->sin_addr.s_addr = inet_addr(ip);

    n = bind(*server_sock, (struct sockaddr *)server_addr, sizeof(*server_addr));
    if (n < 0)
    {
        perror(RED "[-]Bind error" RESET);
        exit(1);
    }
    printf("[+]Bind to the port number: %d\n", port);
    if (listen(*server_sock, 5) == -1)
    {
        perror(RED "[-]Listen error" RESET);
        exit(1);
    }
    else
        printf("[+]Listening...\n");
}

void connect_to_NS_from_SS(int *sock, struct sockaddr_in *addr, const char *ip, int port_num) // Cut
{
    // Create the socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        perror(RED "[-]Socket error" RESET);
        return;
    }

    // Set up the address structure
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port_num);
    addr->sin_addr.s_addr = inet_addr(ip); // Convert IP address to the proper format

    // Bind the socket to the address
    if (bind(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        perror(RED "[-]Bind error" RESET);
        exit(1);
    }

    // Start listening
    if (listen(*sock, 5) == -1)
    {
        perror(RED "[-]Listen error" RESET);
        exit(1);
    }
    else
        printf("[+]Listening...\n");

    return;
}

void connect_to_SS_from_NS(int *ns_sock, struct sockaddr_in *ns_addr, int port_num)
{
    // Create the socket for the naming server
    *ns_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*ns_sock == -1)
    {
        perror("[-]Socket error");
        exit(1);
    }

    // Set up the address structure for the naming server
    ns_addr->sin_family = AF_INET;
    ns_addr->sin_port = htons(port_num);               // Replace with your naming server's port number
    ns_addr->sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with the actual IP address of the storage server

    // Connect to the storage server
    if (connect(*ns_sock, (struct sockaddr *)ns_addr, sizeof(*ns_addr)) == -1)
    {
        perror(RED "[-]Connect error" RESET);
        exit(1);
    }

    printf("[+]Connected to Storage Server.\n");
}

void connect_to_SS_from_client(int *sock, struct sockaddr_in *addr, char *ns_ip, int ns_port)
{
    // Create the socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        perror("[-]Socket error");
        return;
    }

    // Set up the address structure
    addr->sin_family = AF_INET;
    addr->sin_port = htons(ns_port);
    addr->sin_addr.s_addr = inet_addr(ns_ip);
    printf("port: %d\n", ns_port);

    // Connect to the naming server
    if (connect(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        perror(RED "[-]Connect error" RESET);
        exit(1);
    }
    else
    {
        printf("[+]Connected to Storage Server.\n");
    }
    return;
}

void connect_to_client(int *sock, struct sockaddr_in *addr, const char *ip, int port_num) // same as init port create sock
{
    // Create the socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        perror(RED "[-]Socket error" RESET);
        return;
    }

    // Set up the address structure
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port_num);
    addr->sin_addr.s_addr = inet_addr(ip); // Convert IP address to the proper format

    // Bind the socket to the address
    if (bind(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        perror(RED "[-]Bind error" RESET);
        return;
    }

    // Start listening
    if (listen(*sock, 5) == -1)
    {
        perror(RED "[-]Listen error" RESET);
        exit(1);
    }
    else
    {
        printf("[+]Listening...\n");
    }

    return;
}

int initialize_SS(int *ss_sock)
{
    storage_servers vital_info = MakeNode_ss("", 1, 1);
    // NISHITA
    // connect_to_SS_from_NS(ns_sock, ns_addr, 5566);
    // if (send(*ns_sock, "1", sizeof("1"), 0) == -1)
    // {
    //     printf(RED "[-]Send error\n" RESET);
    //     return -1;
    // }
    // char buffer[MAX_NUM_PATHS][2]= {'\0'};
    // NISHITA
    char buffer[MAX_NUM_PATHS] = {'\0'};
    int size;
    if ((size = recv(*ss_sock, buffer, sizeof(buffer), 0)) == -1)
    {
        perror(RED "[-]Receive error\n" RESET);
        return -1;
    }
    buffer[size] = '\0';
    if (recv(*ss_sock, &vital_info->ss_send->client_port, sizeof(vital_info->ss_send->client_port), 0) == -1)
    {
        printf(RED "[-]Receive error\n" RESET);
        return -1;
    }
    if (recv(*ss_sock, &vital_info->ss_send->server_port, sizeof(vital_info->ss_send->server_port), 0) == -1)
    {
        printf(RED "[-]Receive error\n" RESET);
        return -1;
    }
    if ((size = recv(*ss_sock, &vital_info->ss_send->ip_addr, sizeof(vital_info->ss_send->ip_addr), 0)) == -1)
    {
        printf(RED "[-]Receive error\n" RESET);
        return -1;
    }
    vital_info->ss_send->ip_addr[size] = '\0';

    // printf("Recieved vital info from SS\n");
    // printf("Port for client: %d\n", vital_info->ss_send->client_port);
    // printf("Port for NM: %d\n", vital_info->ss_send->server_port);
    // printf("IP: %s\n", vital_info->ss_send->ip_addr);
    // printf("Paths: %s\n", buffer);

    FILE *file = fopen("namethatshallnotbeused.txt", "w");
    fputs(buffer, file);
    fclose(file);

    {
        // char buffer2[15]= {'\0'};
        // file = fopen("temp.txt", "r");
        // FILE *file2 = fopen("temp1.txt", "w");
        // fgets(buffer2, sizeof(buffer2), file);
        // while (fgets(buffer2, sizeof(buffer2), file) != NULL)
        //     fputs(buffer2, file2);

        // fclose(file);
        // fclose(file2);
        // remove("temp.txt");
        // rename("temp1.txt", "temp.txt");
    }

    // listen_for_client(server_sock, client_sock, client_addr, addr_size);
    int num_storage_servers = 1;
    load_SS(vital_info->files_and_dirs, "namethatshallnotbeused.txt");
    vital_info->next = storage_server_list;
    storage_server_list = vital_info;
    remove("namethatshallnotbeused.txt");

    return 0;
}

void make_socket_non_blocking(int socket)
{
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1)
    {
        perror(RED "[-]Failed to get socket flags" RESET);
        exit(1);
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror(RED "[-]Failed to set socket to non-blocking mode" RESET);
        exit(1);
    }
}

void MakeSSsend_vital(int *naming_server_sock, char *ip, int *port_for_client, int *port_for_nm, char *paths_file)
{
    FILE *file = fopen(paths_file, "r");
    if (file == NULL)
    {
        perror(RED "[-] File opening error" RESET);
        exit(1);
    }
    char buffer[MAX_NUM_PATHS] = {'\0'};

    size_t len;
    if ((len = fread(buffer, sizeof(buffer), 1, file)) == -1)
    {
        perror(RED "[-] File reading error" RESET);
        exit(1);
    }
    // buffer[len] = '\0';
    printf("Paths: %s\n", buffer);

    if (send(*naming_server_sock, buffer, sizeof(buffer), 0) == -1)
    {
        perror(RED "[-]Error sending data" RESET);
        exit(1);
    }
    if (send(*naming_server_sock, port_for_client, sizeof(*port_for_client), 0) == -1)
    {
        perror(RED "[-]Error sending data" RESET);
        exit(1);
    }
    if (send(*naming_server_sock, port_for_nm, sizeof(*port_for_nm), 0) == -1)
    {
        perror(RED "[-]Error sending data" RESET);
        exit(1);
    }
    if (send(*naming_server_sock, ip, sizeof(ip), 0) == -1)
    {
        perror(RED "[-]Error sending data" RESET);
        exit(1);
    }
}

void init_port_create_sock(int *sock, struct sockaddr_in *addr, const char *ip, int port_num)
{
    // Create the socket
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        perror(RED "[-]Socket error" RESET);
        return;
    }

    // Set up the address structure
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port_num);
    addr->sin_addr.s_addr = inet_addr(ip); // Convert IP address to the proper format

    // Bind the socket to the address
    if (bind(*sock, (struct sockaddr *)addr, sizeof(*addr)) == -1)
    {
        perror(RED "[-]Bind error" RESET);
        exit(1);
    }

    // Start listening
    if (listen(*sock, 5) == -1)
    {
        perror(RED "[-]Listen error" RESET);
        exit(1);
    }
    else
        printf("[+]Listening...\n");

    return;
}