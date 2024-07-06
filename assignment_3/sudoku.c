#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SIZE 9
#define NUM_THREADS 27 // 9 for rows, 9 for columns, and 9 for sub-grids
#define TRUE 1
#define FALSE 0

typedef struct
{
    int row;
    int column;
    int *result_array;
} parameters;

int sudoku_board[SIZE][SIZE];

// Function to check if a column is valid in sudoku
void *check_column(void *param)
{
    parameters *data = (parameters *)param;
    int column = data->column;
    int *result_array = data->result_array;

    int seen[SIZE] = {0};

    for (int i = 0; i < SIZE; i++)
    {
        if (sudoku_board[i][column] < 1 || sudoku_board[i][column] > SIZE || seen[sudoku_board[i][column] - 1] == 1)
        {
            result_array[column] = FALSE;
            pthread_exit(0);
        }
        seen[sudoku_board[i][column] - 1] = 1;
    }

    result_array[column] = TRUE;
    pthread_exit(0);
}

// Function to check if a row is valid in sudoku
void *check_row(void *param)
{
    parameters *data = (parameters *)param;
    int row = data->row;
    int *result_array = data->result_array;

    int seen[SIZE] = {0};

    for (int i = 0; i < SIZE; i++)
    {
        if (sudoku_board[row][i] < 1 || sudoku_board[row][i] > SIZE || seen[sudoku_board[row][i] - 1] == 1)
        {
            result_array[row] = FALSE;
            pthread_exit(0);
        }
        seen[sudoku_board[row][i] - 1] = 1;
    }

    result_array[row] = TRUE;
    pthread_exit(0);
}

// Function to check if a 3x3 box is valid in sudoku
void *check_sub_grid(void *param)
{
    parameters *data = (parameters *)param;
    int row = data->row;
    int column = data->column;
    int *result_array = data->result_array;

    int seen[SIZE] = {0};

    for (int i = row; i < row + 3; i++)
    {
        for (int j = column; j < column + 3; j++)
        {
            if (sudoku_board[i][j] < 1 || sudoku_board[i][j] > SIZE || seen[sudoku_board[i][j] - 1] == 1)
            {
                result_array[row + column / 3] = FALSE;
                pthread_exit(0);
            }
            seen[sudoku_board[i][j] - 1] = 1;
        }
    }

    result_array[row + column / 3] = TRUE;
    pthread_exit(0);
}

int main()
{
    FILE *file;

    // Open the file for reading
    file = fopen("sample_in_sudoku.txt", "r");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    // Read data and store in the 2D array
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (fscanf(file, "%d", &sudoku_board[i][j]) != 1)
            {
                printf("Error reading data!\n");
                fclose(file);
                return 1;
            }
        }
    }

    // Print sudoku board
    printf("Sudoku Puzzle Solution is: \n");
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf("%d ", sudoku_board[i][j]);
        }
        printf("\n");
    }

    int result_array_columns[SIZE] = {0};
    int result_array_rows[SIZE] = {0};
    int result_array_sub_grids[SIZE] = {0};

    pthread_t threads[NUM_THREADS];
    parameters *data[NUM_THREADS];

    // Create threads to check rows
    for (int i = 0; i < SIZE; i++)
    {
        data[i] = (parameters *)malloc(sizeof(parameters));
        data[i]->row = i;
        data[i]->column = 0;
        data[i]->result_array = result_array_rows;
        pthread_create(&threads[i], NULL, check_row, data[i]);
    }

    // Create threads to check columns
    for (int i = SIZE; i < 2 * SIZE; i++)
    {
        data[i] = (parameters *)malloc(sizeof(parameters));
        data[i]->row = 0;
        data[i]->column = i - SIZE;
        data[i]->result_array = result_array_columns;
        pthread_create(&threads[i], NULL, check_column, data[i]);
    }

    // Create threads to check sub-grids
    int thread_index = 2 * SIZE;
    for (int i = 0; i < SIZE; i += 3)
    {
        for (int j = 0; j < SIZE; j += 3)
        {
            data[thread_index] = (parameters *)malloc(sizeof(parameters));
            data[thread_index]->row = i;
            data[thread_index]->column = j;
            data[thread_index]->result_array = result_array_sub_grids;
            pthread_create(&threads[thread_index], NULL, check_sub_grid, data[thread_index]);
            thread_index++;
        }
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
        free(data[i]);
    }

    // Check results from threads
    int is_valid = TRUE;
    for (int i = 0; i < SIZE; i++)
    {
        if (result_array_rows[i] == FALSE || result_array_columns[i] == FALSE || result_array_sub_grids[i] == FALSE)
        {
            is_valid = FALSE;
            break;
        }
    }

    if (is_valid)
    {
        printf("Sudoku Puzzle is valid\n");
    }
    else
    {
        printf("Sudoku Puzzle is invalid\n");
    }

    // Close the file
    fclose(file);

    return 0;
}
