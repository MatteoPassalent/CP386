#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CMD_LEN 255
#define MAX_NUM_ARGS 5
#define MAX_PARTITIONS 20

typedef struct
{
    int pid;
    int start;
    int size;
} Partition;

void initialize_memory(int size, Partition *partition_memory, int *num_partitions)
{
    partition_memory[0].pid = -1;
    partition_memory[0].start = 0;
    partition_memory[0].size = size;
    *num_partitions = 1;
}

void first_fit(char *pid, int size_needed, Partition *partition_memory, int *num_partitions)
{
    for (int i = 0; i < *num_partitions; i++)
    {
        if (partition_memory[i].pid == -1 && partition_memory[i].size >= size_needed)
        {
            partition_memory[i].pid = atoi(pid + 1);

            if (partition_memory[i].size > size_needed)
            {
                // Split the partition
                for (int j = *num_partitions; j > i; j--)
                {
                    partition_memory[j] = partition_memory[j - 1];
                }
                partition_memory[i + 1].start = partition_memory[i].start + size_needed;
                partition_memory[i + 1].size = partition_memory[i].size - size_needed;
                partition_memory[i + 1].pid = -1;
                partition_memory[i].size = size_needed;
                (*num_partitions)++;
            }
            printf("Successfully allocated %d to process %s.\n", partition_memory[i].size, pid);
            return;
        }
    }
    printf("Insufficient memory to allocate %d bytes.\n", size_needed);
}

void best_fit(char *pid, int size_needed, Partition *partition_memory, int *num_partitions)
{
    int best_index = -1;
    int best_size = 1000000000;

    for (int i = 0; i < *num_partitions; i++)
    {
        if (partition_memory[i].pid == -1 && partition_memory[i].size >= size_needed && partition_memory[i].size < best_size)
        {
            best_index = i;
            best_size = partition_memory[i].size;
        }
    }

    if (best_index != -1)
    {
        int i = best_index;

        partition_memory[i].pid = atoi(pid + 1);
        if (partition_memory[i].size > size_needed)
        {
            // Split the partition
            for (int j = *num_partitions; j > i; j--)
            {
                partition_memory[j] = partition_memory[j - 1];
            }
            partition_memory[i + 1].start = partition_memory[i].start + size_needed;
            partition_memory[i + 1].size = partition_memory[i].size - size_needed;
            partition_memory[i + 1].pid = -1;
            partition_memory[i].size = size_needed;
            (*num_partitions)++;
        }
        printf("Successfully allocated %d to process %s.\n", partition_memory[i].size, pid);
    }
    else
    {
        printf("Insufficient memory to allocate %d bytes.\n", size_needed);
    }
}

void worst_fit(char *pid, int size_needed, Partition *partition_memory, int *num_partitions)
{
    int worst_index = -1;
    int worst_size = -1;

    for (int i = 0; i < *num_partitions; i++)
    {
        if (partition_memory[i].pid == -1 && partition_memory[i].size >= size_needed && partition_memory[i].size > worst_size)
        {
            worst_index = i;
            worst_size = partition_memory[i].size;
        }
    }

    if (worst_index != -1)
    {
        int i = worst_index;
        partition_memory[i].pid = atoi(pid + 1);

        if (partition_memory[i].size > size_needed)
        {
            // Split the partition
            for (int j = *num_partitions; j > i; j--)
            {
                partition_memory[j] = partition_memory[j - 1];
            }
            partition_memory[i + 1].start = partition_memory[i].start + size_needed;
            partition_memory[i + 1].size = partition_memory[i].size - size_needed;
            partition_memory[i + 1].pid = -1;
            partition_memory[i].size = size_needed;
            (*num_partitions)++;
        }
        printf("Successfully allocated %d to process %s.\n", partition_memory[i].size, pid);
    }
    else
    {
        printf("Insufficient memory to allocate %d bytes.\n", size_needed);
    }
}

void request_memory(char *pid, int size_needed, char algorithm, Partition *partition_memory, int *num_partitions)
{
    if (algorithm == 'f')
    {
        first_fit(pid, size_needed, partition_memory, num_partitions);
    }
    else if (algorithm == 'b')
    {
        best_fit(pid, size_needed, partition_memory, num_partitions);
    }
    else if (algorithm == 'w')
    {
        worst_fit(pid, size_needed, partition_memory, num_partitions);
    }
    else
    {
        printf("Unknown algorithm. Use 'f' for first fit, 'b' for best fit, or 'w' for worst fit.\n");
    }
}

void release_memory(char *pid, Partition *partition_memory, int *num_partitions)
{
    int pid_num = atoi(pid + 1);

    for (int i = 0; i < *num_partitions; i++)
    {
        if (partition_memory[i].pid == pid_num)
        {
            partition_memory[i].pid = -1;
            printf("releasing memory for process %s.\n", pid);

            // Check if the released block can be merged with adjacent free blocks
            if (i > 0 && partition_memory[i - 1].pid == -1)
            {
                partition_memory[i - 1].size += partition_memory[i].size;
                for (int j = i; j < *num_partitions - 1; j++)
                {
                    partition_memory[j] = partition_memory[j + 1];
                }
                (*num_partitions)--;
                i--;
            }
            if (i < *num_partitions - 1 && partition_memory[i + 1].pid == -1)
            {
                partition_memory[i].size += partition_memory[i + 1].size;
                for (int j = i + 1; j < *num_partitions - 1; j++)
                {
                    partition_memory[j] = partition_memory[j + 1];
                }
                (*num_partitions)--;
            }
            printf("Successfully released memory for process %s\n", pid);
            return;
        }
    }
    printf("Process with PID %s not found.\n", pid);
}

void print_status(Partition *partition_memory, int free_head)
{
    int allocated_memory = 0;
    int free_memory = 0;
    for (int i = 0; i < free_head; i++)
    {
        if (partition_memory[i].pid != -1)
        {
            allocated_memory += partition_memory[i].size;
        }
        else
        {
            free_memory += partition_memory[i].size;
        }
    }
    printf("Partitions [Allocated memory= %d]\n", allocated_memory);
    for (int i = 0; i < free_head; i++)
    {
        if (partition_memory[i].pid != -1)
        {
            printf("Address [%d:%d] Process P%d\n", partition_memory[i].start, partition_memory[i].start + partition_memory[i].size - 1, partition_memory[i].pid);
        }
    }
    printf("\n");
    printf("Holes [Free memory= %d]\n", free_memory);
    for (int i = 0; i < free_head; i++)
    {
        if (partition_memory[i].pid == -1)
        {
            printf("Address [%d:%d] len = %d\n", partition_memory[i].start, partition_memory[i].start + partition_memory[i].size - 1, partition_memory[i].size);
        }
    }
}

void compact_memory(Partition *partition_memory, int *num_partitions)
{
    int free_start = 0;

    // Shift all allocated partitions to the left
    for (int i = 0; i < *num_partitions; i++)
    {
        if (partition_memory[i].pid != -1)
        {
            if (free_start != i)
            {
                partition_memory[free_start] = partition_memory[i];
                partition_memory[free_start].start = (free_start == -1) ? 0 : partition_memory[free_start - 1].start + partition_memory[free_start - 1].size;
            }
            free_start++;
        }
    }

    // Create a single free partition at the end
    if (free_start < *num_partitions)
    {
        partition_memory[free_start].pid = -1;
        partition_memory[free_start].start = (free_start == -1) ? 0 : partition_memory[free_start - 1].start + partition_memory[free_start - 1].size;
        partition_memory[free_start].size = partition_memory[*num_partitions - 1].start + partition_memory[*num_partitions - 1].size - partition_memory[free_start].start;
        *num_partitions = free_start + 1;
    }
    printf("Compaction process is successful\n");
}

void tolower_string(char str[])
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        str[i] = tolower(str[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <memory_size_in_bytes>\n", argv[0]);
        return 1;
    }

    int memory_size = atoi(argv[1]);
    Partition *partition_memory = malloc(MAX_PARTITIONS * sizeof(Partition));
    int num_partitions = 0;

    initialize_memory(memory_size, partition_memory, &num_partitions);

    char cmd[MAX_CMD_LEN];
    char *token;

    while (1)
    {
        printf("allocator> ");
        fgets(cmd, MAX_CMD_LEN, stdin);

        cmd[strcspn(cmd, "\n")] = 0;

        token = strtok(cmd, " ");

        tolower_string(token);

        if (token == NULL)
        {
            continue;
        }

        if (strcmp(token, "exit") == 0)
        {
            free(partition_memory);
            exit(0);
        }
        else if (strcmp(token, "rq") == 0)
        {
            char pid[4];
            int size_needed;
            char algorithm;

            token = strtok(NULL, " ");
            if (token != NULL)
            {
                strncpy(pid, token, sizeof(pid) - 1);
                pid[sizeof(pid) - 1] = '\0';
            }
            else
            {
                printf("Invalid command format. Usage: RQ <PID> <size> <algorithm>\n");
                continue;
            }

            token = strtok(NULL, " ");
            if (token != NULL)
            {
                size_needed = atoi(token);
            }
            else
            {
                printf("Invalid command format. Usage: RQ <PID> <size> <algorithm>\n");
                continue;
            }

            token = strtok(NULL, " ");
            if (token != NULL)
            {
                algorithm = tolower(token[0]);
            }
            else
            {
                printf("Invalid command format. Usage: RQ <PID> <size> <algorithm>\n");
                continue;
            }

            request_memory(pid, size_needed, algorithm, partition_memory, &num_partitions);
        }
        else if (strcmp(token, "rl") == 0)
        {
            char pid[4];

            token = strtok(NULL, " ");
            if (token != NULL)
            {
                strncpy(pid, token, sizeof(pid) - 1);
                pid[sizeof(pid) - 1] = '\0';
            }
            else
            {
                printf("Invalid command format. Usage: RL <PID>\n");
                continue;
            }

            release_memory(pid, partition_memory, &num_partitions);
        }
        else if (strcmp(token, "status") == 0)
        {
            print_status(partition_memory, num_partitions);
        }
        else if (strcmp(token, "c") == 0)
        {
            compact_memory(partition_memory, &num_partitions);
        }
        else
        {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
