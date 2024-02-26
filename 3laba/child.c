#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <locale.h>

int main(int argc, char *argv[], char *envp[])
{
    while (1)
    {
        printf("Parent process: ");
        char input;
        scanf(" %c", &input);
        if (input == '+')
        {
            pid_t pid = fork();
        }
    }

    return 0;
}