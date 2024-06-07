#include <stdio.h>

int main(int argc, char *argv[])
{

    if (argc <= 2)
    {
        printf("Insufficient parameters passed.\n");
        return 1;
    }

    char *input_file_name = argv[1];
    char *output_file_name = argv[2];

    FILE *input_file = fopen(input_file_name, "r");
    FILE *output_file = fopen(output_file_name, "w");

    if (input_file == NULL)
    {
        printf("Error opening input file: %s\n", input_file_name);
        return 1;
    }

    char line[255];

    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        fprintf(output_file, "%s", line);
    }

    fclose(input_file);
    fclose(output_file);

    printf("The contents of file 'input.txt' have been successfully copied into 'output.txt' file.\n");

    return 0;
}