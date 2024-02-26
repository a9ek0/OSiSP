#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <locale.h>

volatile sig_atomic_t flag = 1;
pid_t lastChildPid;
int remainingChildCount = 0;

void killHandler(int signo)
{
    if (signo == SIGCHLD)
    {
        printf("Child process with PID %d terminated.\n", lastChildPid);
        remainingChildCount--;
    }
}

void parentHandler(int signo)
{
    flag = 1;
}

void childHandler(int signo)
{
    flag = 0;
}

int main(int argc, char *argv[], char *envp[])
{
    signal(SIGCHLD, killHandler);
    signal(SIGUSR1, parentHandler);
    signal(SIGUSR2, childHandler);

    while (1)
    {
        printf("Parent process: ");
        char input;

        // char c;
        // while ((c = getchar()) != '\n' || c != EOF);

        fflush(stdin);
        scanf(" %c", &input);
        if (input == '+')
        {
            pid_t childPid = fork();
            if (childPid == -1)
            {
                perror("fork");

                lastChildPid = getpid();

                exit(EXIT_FAILURE);
            }
            else if (childPid == 0)
            {
                printf("SISKI\n");
            }
            else
            {

                lastChildPid = childPid;
                printf("Parent process created child with PID %d.\n", lastChildPid);
                remainingChildCount++;

                waitpid(lastChildPid, NULL, 0);
            
            }
        }
        else if (input == '-')
        {
            if (lastChildPid > 0)
            {
                kill(lastChildPid, SIGTERM);
                waitpid(lastChildPid, NULL, 0);
                printf("Remaining: %d\n", remainingChildCount);
                lastChildPid = 0;
            }
            else
            {
                printf("No child process to terminate.\n");
            }
        }
        else if (input == 'q')
        {
            break;
        }
        else
        {
            printf("GOVNO JOPA");
        }
    }

    return 0;
}