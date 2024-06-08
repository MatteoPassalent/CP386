#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_NUMBERS 100
#define SHM_NAME "/my_memory"

void generate_collatz_sequence(int start_number, int *sequence, int *seq_length)
{
    int num = start_number;
    int length = 0;

    while (num != 1)
    {
        sequence[length++] = num;
        if (num % 2 == 0)
        {
            num = num / 2;
        }
        else
        {
            num = 3 * num + 1;
        }
    }
    sequence[length++] = 1;
    *seq_length = length;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s start_numbers.txt\n", argv[0]);
        exit(1);
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    int start_numbers[MAX_NUMBERS];
    int num_start_numbers = 0;

    // Read start numbers from file
    while (num_start_numbers < MAX_NUMBERS && fscanf(file, "%d", &start_numbers[num_start_numbers]) == 1)
    {
        num_start_numbers++;
    }

    fclose(file);

    // Shared memory size to accommodate the sequence and its length
    size_t shm_size = sizeof(int) * (MAX_NUMBERS + 1);

    // Loop through each start number
    for (int i = 0; i < num_start_numbers; i++)
    {
        int shm_fd;
        int *shm_ptr;

        // Create shared memory object
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1)
        {
            perror("shm_open failed");
            exit(1);
        }

        if (ftruncate(shm_fd, shm_size) == -1)
        {
            perror("ftruncate failed");
            exit(1);
        }

        shm_ptr = mmap(0, shm_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED)
        {
            perror("mmap failed");
            exit(1);
        }

        int *sequence = shm_ptr;
        int *seq_length = shm_ptr + MAX_NUMBERS;

        printf("Parent Process: The positive integer read from file is %d\n", start_numbers[i]);

        int pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }
        else if (pid == 0)
        { // Child process
            printf("Child Process: The generated collatz sequence is ");
            generate_collatz_sequence(start_numbers[i], sequence, seq_length);
            for (int j = 0; j < *seq_length; j++)
            {
                printf("%d ", sequence[j]);
            }
            printf("\n");
            exit(0);
        }
        else
        { // Parent process
            wait(NULL);
            // Unlink shared memory object after use
            if (munmap(shm_ptr, shm_size) == -1)
            {
                perror("munmap failed");
                exit(1);
            }
            if (shm_unlink(SHM_NAME) == -1)
            {
                perror("shm_unlink failed");
                exit(1);
            }
        }
    }

    return 0;
}
