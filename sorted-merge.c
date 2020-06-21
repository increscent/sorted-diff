#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void close_file(FILE*, const char*);
size_t max(size_t, size_t);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: sorted-merge [base] [branch]\n");
        return 1;
    }

    FILE *base_file = fopen(argv[1], "r");
    FILE *brch_file = fopen(argv[2], "r");

    char *base_buffer = malloc(1024);
    char *brch_buffer = malloc(1024);

    size_t base_n = 1024;
    size_t brch_n = 1024;

    ssize_t base_read_result = getline(&base_buffer, &base_n, base_file);
    ssize_t brch_read_result = getline(&brch_buffer, &brch_n, brch_file);

    int compare = 0;

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

        if (compare < 0)
        {
            // branch skipped base; base line was deleted
            fwrite(base_buffer, strlen(base_buffer), 1, stdout);
            base_read_result = getline(&base_buffer, &base_n, base_file);
        }
        else if (compare > 0)
        {
            // base does not contain branch line; branch line was added
            fwrite(brch_buffer, strlen(brch_buffer), 1, stdout);
            brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
        }
        else
        {
            // no additions or deletions; lines match
            if (time_base >= time_brch)
            {
                fwrite(base_buffer, strlen(base_buffer), 1, stdout);
            }
            else
            {
                fwrite(brch_buffer, strlen(brch_buffer), 1, stdout);
            }
            base_read_result = getline(&base_buffer, &base_n, base_file);
            brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
        }
    }

    while (base_read_result >= 0)
    {
        // all lines left over in base have been deleted
        fwrite(base_buffer, strlen(base_buffer), 1, stdout);
        base_read_result = getline(&base_buffer, &base_n, base_file);
    }

    while (brch_read_result >= 0)
    {
        // all lines left over in branch have been added
        fwrite(brch_buffer, strlen(brch_buffer), 1, stdout);
        brch_read_result = getline(&brch_buffer, &brch_n, brch_file);
    }

    close_file(base_file, argv[1]);
    close_file(brch_file, argv[2]);

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
