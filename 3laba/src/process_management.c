#include "process_management.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

extern int kill(pid_t pid, int sig);

void allocate_child_processes()
{
    child_processes = (process_info *)calloc(max_child_processes, sizeof(process_info));
    if(!child_processes)
    {
        exit(errno);
    }
}

void create_child_process()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "%s, error code: %d\n", strerror(errno), errno);
        exit(errno);
    }
    if (pid == 0) execl(child_name, child_name, NULL);
    else
    {
        num_child_processes++;
        if(num_child_processes >= max_child_processes)
        {
            max_child_processes *= 2;
            process_info *tmp = (process_info *)realloc(child_processes, max_child_processes);
            if(!tmp)
            {
                exit(errno);
            }
            child_processes = tmp;
            tmp = NULL;
        }
        child_init(pid);
        printf("%s with pid = %d was created\n", get_last_child_process().name, pid);
        printf("size of child_processes - %d\n", (int) num_child_processes);
    }
}

void terminate_last_child_process()
{
    delete_child_process(get_last_child_process().pid);
    printf("size of child_processes - %d\n", (int) num_child_processes);
}

void list_all_processes()
{
    printf("parent:\nP    with pid = %d\n", (int) getpid());
    printf("children :\n");
    for (int i = 0; i < (int) num_child_processes; i++)
    {
        printf("%s with pid = %d is ", child_processes[i].name, child_processes[i].pid);
        if (child_processes[i].is_stopped) printf("stopped\n");
        else printf("running\n");
    }
}

void terminate_all_child_processes()
{
    remove_all_child_processes();
}

void stop_child_process(int index)
{
    if(index != -1)         // выполняем опцию для конкретного процесса
    {
        kill(child_processes[index].pid, SIGUSR1);
        child_processes[index].is_stopped = true;
        printf("%s with pid = %d was stopped\n", child_processes[index].name, child_processes[index].pid);
        return;
    }
    for(int i = 0; i < (int) num_child_processes; i++)
    {
        kill(child_processes[i].pid, SIGUSR1);
        child_processes[i].is_stopped = true;
        printf("%s with pid = %d was stopped\n", child_processes[i].name, child_processes[i].pid);
    }
}

void resume_child_process(int index)
{
    alarm(0);               // выключаем будильник, если он есть
    if(index != -1)         // выполняем опцию для конкретного процесса
    {
        kill(child_processes[index].pid, SIGUSR2);
        child_processes[index].is_stopped = false;
        printf("%s with pid = %d is running now\n", child_processes[index].name, child_processes[index].pid);
        return;
    }
    for(int i = 0; i < (int) num_child_processes; i++)
    {
        kill(child_processes[i].pid, SIGUSR2);
        child_processes[i].is_stopped = false;
        printf("%s with pid = %d is running now\n", child_processes[i].name, child_processes[i].pid);
    }
}

void quit_program()
{
    terminate_all_child_processes();
    if(child_processes) free(child_processes);
    child_processes = NULL;
    exit(0);
}

int get_process_index_by_pid(pid_t pid)
{
    for(int i = 0; i < (int) num_child_processes; i++)
    {
        if(child_processes[i].pid == pid) return i;
    }
    return -1;
}



char *get_process_name_by_pid(pid_t pid)
{
    for(int i = 0; i < (int) num_child_processes; i++)
    {
        if(child_processes[i].pid == pid) return child_processes[i].name;
    }
    return NULL;
}


void child_init(pid_t pid)
{
    sprintf(child_processes[num_child_processes - 1].name, "C_%02d", (int) num_child_processes - 1);
    child_processes[num_child_processes - 1].pid = pid;
    child_processes[num_child_processes - 1].is_stopped = true;
}

process_info get_last_child_process()
{
    return child_processes[num_child_processes - 1];
}

void delete_child_process(pid_t pid)
{
    kill(pid, SIGTERM);
    printf("%s with pid = %d was deleted\n", get_last_child_process().name, get_last_child_process().pid);
    num_child_processes--;
}

void remove_all_child_processes()
{
    while(num_child_processes) delete_child_process(get_last_child_process().pid);
    printf("all child processes were successfully deleted\n");
}

void prioritize_child_process(int index)
{
    stop_child_process(-1);
    resume_child_process(index);
    alarm(5);
}