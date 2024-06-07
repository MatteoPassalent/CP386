#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void displayMenu();
int handleUserChoice(char choice);
void createDirectory();
void removeDirectory();
void printCurrentDirectory();
void changeToParentDirectory();
void readDirectoryContent();
void closeDirectory();

DIR *currentDir = NULL;

int main()
{
    displayMenu();
    return 0;
}

void displayMenu()
{
    char choice[3];
    while (1)
    {
        printf("Select the option(s) appropriately by entering the number:\n");
        printf("1. Create a directory\n");
        printf("2. Remove a directory\n");
        printf("3. Print current working directory\n");
        printf("4. Change directory one level up\n");
        printf("5. Read the content of the directory\n");
        printf("6. Close the current directory\n");
        printf("q. Exit the program\n");
        fgets(choice, sizeof(choice), stdin);
        int exit = handleUserChoice(choice[0]);
        if (exit)
            break;
    }
}

int handleUserChoice(char choice)
{
    switch (choice)
    {
    case '1':
        createDirectory();
        break;
    case '2':
        removeDirectory();
        break;
    case '3':
        printCurrentDirectory();
        break;
    case '4':
        changeToParentDirectory();
        break;
    case '5':
        readDirectoryContent();
        break;
    case '6':
        closeDirectory();
        break;
    case 'q':
        printf("Exit the program selected.\n");
        return 1;
    default:
        printf("Invalid choice. Please try again.\n");
    }

    return 0;
}

void createDirectory()
{
    printf("Enter the name of the directory you want to create: \n");
    char directoryName[100];
    fgets(directoryName, sizeof(directoryName), stdin);
    directoryName[strcspn(directoryName, "\n")] = '\0';
#ifdef _WIN32
    int status = mkdir(directoryName);
#else
    int status = mkdir(directoryName, 0755);
#endif
    if (status == 0)
        printf("Directory created successfully.\n\n");
    else
    {
        printf("Failed to create the directory.\n\n");
    }
}

void removeDirectory()
{
    printf("Enter the name of the directory you want to remove: \n");
    char directoryName[100];
    fgets(directoryName, sizeof(directoryName), stdin);
    directoryName[strcspn(directoryName, "\n")] = '\0';
    int status = rmdir(directoryName);
    if (status == 0)
        printf("Directory removed successfully.\n\n");
    else
    {
        printf("Directory not found.\n\n");
    }
}

void printCurrentDirectory()

{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n\n", cwd);
    }
    else
    {
        printf("Failed to get current directory\n\n");
    }
}

void changeToParentDirectory()
{
    if (chdir("..") == 0)
    {
        printf("Changed to parent directory successfully.\n\n");
    }
    else
    {
        printf("Failed to change to parent directory.\n\n");
    }
}

void readDirectoryContent()
{
    char path[1024];
    getcwd(path, sizeof(path));
    path[strcspn(path, "\n")] = '\0';

    if (currentDir != NULL)
    {
        printf("A directory is already open. Please close it first.\n\n");
        return;
    }

    if ((currentDir = opendir(path)) == NULL)
    {
        printf("Failed to open directory\n\n");
    }
    else
    {
        struct dirent *entry;
        printf("Contents of %s:\n", path);
        while ((entry = readdir(currentDir)) != NULL)
        {
            printf("%s\n", entry->d_name);
        }
    }
    printf("\n");
}

void closeDirectory()
{
    if (currentDir == NULL)
    {
        printf("No directory is currently open.\n\n");
    }
    else
    {
        if (closedir(currentDir) == -1)
        {
            printf("Failed to close the directory.\n\n");
        }
        else
        {
            printf("Directory closed successfully.\n\n");
            currentDir = NULL;
        }
    }
}