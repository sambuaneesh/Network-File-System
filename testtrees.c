#include "header.h"

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

Tree Search_Till_Parent(Tree T, char *path)
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
            parent = Insert(parent, path_duplicate2 + strlen(so_far));
            return T;
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

    return traveller->parent;
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

void Delete_Path(Tree T, char *path)
{
    Tree traveller = Search_Till_Parent(T, path);
    if (traveller == NULL)
    {
        printf("Path not found\n");
        return;
    }
    if(traveller->parent == NULL)
    {
        printf("Cannot delete root\n");
        return;
    }
    if (traveller->parent->first_child == traveller)
        traveller->parent->first_child = traveller->next_sibling;
    if (traveller->prev_sibling != NULL)
        traveller->prev_sibling->next_sibling = traveller->next_sibling; // diconnected the dir now
    
    Del_Rec(traveller->first_child);
    free(traveller);
}

int main()
{
    Tree T = MakeNode(".");

    // T = Search_Till_Parent(T, "A");
    // T = Search_Till_Parent(T, "B/B1");
    // T = Search_Till_Parent(T, "B/B2");
    // T = Search_Till_Parent(T, "C");
    // T = Search_Till_Parent(T, "D/D1");
    // T = Search_Till_Parent(T, "E/E1/E2");
    // T = Search_Till_Parent(T, "E/E3");
    // Delete_Path(T, "B");
    // Delete_Path(T, "E/E1");
    T = Search_Till_Parent(T, "home/dell/gitrepo/file1");
    T = Search_Till_Parent(T, "home/dell/gitrepo/file2");
    T = Search_Till_Parent(T, "home/dell/gitrepo/file3");
    PrintTree(T);
    return 0;
}