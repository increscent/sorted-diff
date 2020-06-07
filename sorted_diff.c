#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void close_file(FILE*, const char*);

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        printf("Usage: sorted-diff [base] [branch] [additions] [deletions]\n");
        return 1;
    }

    FILE *base_file = fopen(argv[1], "r");
    FILE *branch_file = fopen(argv[2], "r");

    FILE *additions_file = fopen(argv[3], "w");
    FILE *deletions_file = fopen(argv[4], "w");

    char* base_buffer = malloc(1024);
    char* branch_buffer = malloc(1024);

    size_t base_n = 1024;
    size_t branch_n = 1024;

    ssize_t base_read_result = getline(&base_buffer, &base_n, base_file);
    ssize_t branch_read_result = getline(&branch_buffer, &branch_n, branch_file);

    int compare = 0;
    int additions = 0;
    int deletions = 0;

    while (base_read_result >= 0 && branch_read_result >= 0)
    {
        compare = strcmp(base_buffer, branch_buffer);
        if (compare < 0)
        {
            // branch skipped base; base file was deleted
            fwrite(base_buffer, strlen(base_buffer), 1, deletions_file);
            base_read_result = getline(&base_buffer, &base_n, base_file);
            deletions++;
        }
        else if (compare > 0)
        {
            // base does not contain branch file; branch file was added
            fwrite(branch_buffer, strlen(branch_buffer), 1, additions_file);
            branch_read_result = getline(&branch_buffer, &branch_n, branch_file);
            additions++;
        }
        else
        {
            // no additions or deletions; files match
            base_read_result = getline(&base_buffer, &base_n, base_file);
            branch_read_result = getline(&branch_buffer, &branch_n, branch_file);
        }
    }

    while (getline(&base_buffer, &base_n, base_file) >= 0)
    {
        // all files left over in base have been deleted
        fwrite(base_buffer, strlen(base_buffer), 1, deletions_file);
        deletions++;
    }

    while (getline(&branch_buffer, &branch_n, branch_file) >= 0)
    {
        // all files left over in branch have been added
        fwrite(branch_buffer, strlen(branch_buffer), 1, additions_file);
        additions++;
    }

    printf("%d addition(s)\n", additions);
    printf("%d deletion(s)\n", deletions);

    close_file(base_file, argv[1]);
    close_file(branch_file, argv[2]);

    close_file(additions_file, argv[3]);
    close_file(deletions_file, argv[4]);

    return 0;
}

void close_file(FILE *f, const char *name)
{
    if (fclose(f))
    {
        fprintf(stderr, "Error closing this file: %s\n", name);
    }
}
