#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

void writeOutput(char *command, char *output);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    char *shm_ptr = (char *)shmat(shmid, (void *)0, 0);

    if (fork() == 0)
    {
        FILE *file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("Error: File not found\n");
            return 1;
        }
        char line[256];
        while (fgets(line, sizeof(line), file))
        {
            strcat(shm_ptr, line);
        }
        fclose(file);
        shmdt(shm_ptr);
        exit(0);
    }
    wait(NULL);

    char *commands = strdup(shm_ptr);

    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    char *command = strtok(commands, "\n");
    while (command != NULL)
    {
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            perror("pipe failed");
            return 1;
        }

        if (fork() == 0)
        {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            char *args[] = {"/bin/sh", "-c", command, NULL};
            execvp(args[0], args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }

        wait(NULL);
        close(pipefd[1]);
        char buffer[1024];
        int bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            buffer[bytes_read] = '\0';
            writeOutput(command, buffer);
        }
        close(pipefd[0]);
        command = strtok(NULL, "\n");
    }
    free(commands);
    return 0;
}
void writeOutput(char *command, char *output)
{
    FILE *fp;
    fp = fopen("output.txt", "a");
    fprintf(fp, "The output of: %s : is\n", command);
    fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);
    fclose(fp);
}