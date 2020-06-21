#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOR_MODE 1
#define MOD_MODE 2

void close_file(FILE*, const char*);
size_t max(size_t, size_t);

int main(int argc, char **argv)
{
    if (argc < 6)
    {
        printf("Usage: sorted-diff [normal|modified] [base] [branch] [additions] [deletions]\n");
        return 1;
    }

    int mode = strcmp(argv[1], "modified") == 0 ? MOD_MODE : NOR_MODE;

    FILE *base_file = fopen(argv[2], "r");
    FILE *brch_file = fopen(argv[3], "r");

    FILE *additions_file = fopen(argv[4], "w");
    FILE *deletions_file = fopen(argv[5], "w");

    char *base_buffer = malloc(1024);
    char *brch_buffer = malloc(1024);

    size_t base_n = 1024;
    size_t brch_n = 1024;

    ssize_t base_read_result = getline(&base_buffer, &base_n, base_file);
    ssize_t brch_read_result = getline(&brch_buffer, &brch_n, brch_file);

    int compare = 0;
    int additions = 0;
    int deletions = 0;

    char *tab_base;
    char *tab_brch;

    size_t len_base;
    size_t len_brch;

    long time_base;
    long time_brch;

    while (base_read_result >= 0 && brch_read_result >= 0)
    {
        tab_base = strchr(base_buffer, '\t');
        tab_brch = strchr(brch_buffer, '\t');

        len_base = tab_base == NULL ? strlen(base_buffer) : tab_base - base_buffer;
        len_brch = tab_brch == NULL ? strlen(brch_buffer) : tab_brch - brch_buffer;

        time_base = tab_base == NULL ? 0 : atol(tab_base + 1);
        time_brch = tab_brch == NULL ? 0 : atol(tab_brch + 1);

        compare = strncmp(base_buffer, brch_buffer, max(len_base, len_brch));

        if (mode == MOD_MODE && compare == 0 && time_brch >= time_base) {
            // This counts as an addition (it's a modification)
            compare = 1;
        }

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
            fwrite(brch_buffer, strlen(brch_buffer), 1, additions_file);
            brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
            additions++;
        }
        else
        {
            // no additions or deletions; files match
            base_read_result = getline(&base_buffer, &base_n, base_file);
            brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
        }
    }

    while (base_read_result >= 0)
    {
        // all files left over in base have been deleted
        fwrite(base_buffer, strlen(base_buffer), 1, deletions_file);
        deletions++;
        base_read_result = getline(&base_buffer, &base_n, base_file);
    }

    while (brch_read_result >= 0)
    {
        // all files left over in branch have been added
        fwrite(brch_buffer, strlen(brch_buffer), 1, additions_file);
        additions++;
        brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
    }

    printf("%d addition(s)\n", additions);
    printf("%d deletion(s)\n", deletions);

    close_file(base_file, argv[1]);
    close_file(brch_file, argv[2]);

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

size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}
