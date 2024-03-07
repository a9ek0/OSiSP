#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <getopt.h>

#define PATH_MAX 4096

extern int scandir(const char *dirp, struct dirent ***namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));
extern int alphasort();
extern int lstat(const char *restrict path, struct stat *restrict buf); 

int compareStrings(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

void dirwalk(const char *dir_path, bool symbolic, bool directories, bool files, bool sort);
const char *getDir(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    const char *startDir = getDir(argc, argv);
    printf("%s\n", startDir);

    int ln = 0;
    int dir = 0;
    int fl = 0;
    int sort = 0;

    char opt;

    while ((opt = getopt(argc, argv, "ldfs")) != -1)
    {
        if (opt == 'l')
        {
            ln = 1;
        }
        else if (opt == 'd')
        {
            dir = 1;
        }
        else if (opt == 'f')
        {
            fl = 1;
        }
        else if (opt == 's')
        {
            sort = 1;
        }
    }
    
    dirwalk(startDir, ln, dir, fl, sort);

    return 0;
}

void dirwalk(const char *dir_path, bool symbolic, bool directories, bool files, bool sort)
{
    struct dirent **nameList;
    int numEntries;

    numEntries = sort ? scandir(dir_path, &nameList, NULL, alphasort) : scandir(dir_path, &nameList, NULL, NULL);

    if (numEntries == -1)
    {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numEntries; i++)
    {
        if (strcmp(nameList[i]->d_name, ".") == 0 || strcmp(nameList[i]->d_name, "..") == 0)
        {
            continue;
        }

        char path[PATH_MAX];

        snprintf(path, sizeof(path), "%s/%s", dir_path, nameList[i]->d_name);

        struct stat stats;
        if (lstat(path, &stats) == -1)
        {
            perror("lstat");
            continue;
        }

        if (symbolic && S_ISLNK(stats.st_mode))
        {
            printf("%s\n", path);
        }
        else if (directories && S_ISDIR(stats.st_mode))
        {
            printf("%s\n", path);
        }
        else if (files && S_ISREG(stats.st_mode))
        {
            printf("%s\n", path);
        }
        else if (symbolic == 0 && directories == 0 && files == 0)
        {
            printf("%s\n", path);
        }

        if (S_ISDIR(stats.st_mode))
        {
            dirwalk(path, symbolic, directories, files, sort);
        }

        free(nameList[i]);
    }

    free(nameList);
}

const char *getDir(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            continue;
        }
        else
        {
            if (argv[i][strlen(argv[i]) - 1] == '/')
            {
                argv[i][strlen(argv[i]) - 1] = '\0';
            }
            return argv[i];
        }
    }
    
    return ".";
}