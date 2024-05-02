#include "process_management.h"
#include "globals.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

bool received_signal = false;
pair stats;
size_t c00 = 0, c01 = 0, c10 = 0, c11 = 0;
child_state state = WAITING;
size_t num_child_processes = 0;
size_t max_child_processes = MAX_CHILDREN;
process_info *child_processes = NULL;
char child_name[CHILD_NAME_LENGTH] = "./child";

void allocate_child_processes(void) {
    child_processes = (process_info *) calloc(max_child_processes, sizeof(process_info));
    if (!child_processes) {
        exit(errno);
    }
}

void create_child_process(void) {
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "%s, error code: %d\n", strerror(errno), errno);
        exit(errno);
    }
    if (pid == 0) {
        execl(child_name, child_name, NULL);
    } else {
        num_child_processes++;
        if (num_child_processes >= max_child_processes) {
            max_child_processes *= 2;
            process_info *tmp = (process_info *) realloc(child_processes, max_child_processes);
            if (!tmp) {
                exit(errno);
            }
            child_processes = tmp;
            tmp = NULL;
        }

        sprintf(child_processes[num_child_processes - 1].name, "C_%02d", (int) num_child_processes - 1);
        child_processes[num_child_processes - 1].pid = pid;
        child_processes[num_child_processes - 1].is_stopped = true;

        printf("%s with pid = %d was created\n", get_last_child_process().name, pid);
        printf("number of child processes - %d\n", (int) num_child_processes);
    }
}

void terminate_last_child_process(void) {
    kill(get_last_child_process().pid, SIGTERM);
    printf("%s with pid = %d was deleted\n", get_last_child_process().name, get_last_child_process().pid);
    num_child_processes--;
    printf("number of child processes - %d\n", (int) num_child_processes);
}

void list_all_processes(void) {
    printf("parent:\nP    with pid = %d\n", (int) getpid());
    printf("children :\n");
    for (int i = 0; i < (int) num_child_processes; i++) {
        printf("%s with pid = %d is ", child_processes[i].name, child_processes[i].pid);
        if (child_processes[i].is_stopped) {
            printf("stopped\n");
        } else {
            printf("running\n");
        }
    }
}

void stop_child_process(int index) {
    if (index != -1) {
        kill(child_processes[index].pid, SIGUSR1);
        child_processes[index].is_stopped = true;
        printf("%s with pid = %d was stopped\n", child_processes[index].name, child_processes[index].pid);
        return;
    }

    for (int i = 0; i < (int) num_child_processes; i++) {
        kill(child_processes[i].pid, SIGUSR1);
        child_processes[i].is_stopped = true;
        printf("%s with pid = %d was stopped\n", child_processes[i].name, child_processes[i].pid);
    }
}

void resume_child_process(int index) {
    alarm(0);
    if (index != -1) {
        kill(child_processes[index].pid, SIGUSR2);
        child_processes[index].is_stopped = false;
        printf("%s with pid = %d is running now\n", child_processes[index].name, child_processes[index].pid);
        return;
    }

    for (int i = 0; i < (int) num_child_processes; i++) {
        kill(child_processes[i].pid, SIGUSR2);
        child_processes[i].is_stopped = false;
        printf("%s with pid = %d is running now\n", child_processes[i].name, child_processes[i].pid);
    }
}

void quit_program(void) {
    remove_all_child_processes();
    if (child_processes) {
        free(child_processes);
    }
    child_processes = NULL;
    exit(0);
}

int get_process_index_by_pid(pid_t pid) {
    for (int i = 0; i < (int) num_child_processes; i++) {
        if (child_processes[i].pid == pid) {
            return i;
        }
    }
    return -1;
}


char *get_process_name_by_pid(pid_t pid) {
    for (int i = 0; i < (int) num_child_processes; i++) {
        if (child_processes[i].pid == pid) {
            return child_processes[i].name;
        }
    }
    return NULL;
}

process_info get_last_child_process() {
    return child_processes[num_child_processes - 1];
}

void remove_all_child_processes(void) {
    while (num_child_processes) {
        kill(get_last_child_process().pid, SIGTERM);
        printf("%s with pid = %d was deleted\n", get_last_child_process().name, get_last_child_process().pid);
        num_child_processes--;
    }
    printf("all child processes were successfully deleted\n");
}

void prioritize_child_process(int index) {


    for (int i = 0; i < (int) num_child_processes; i++) {
        if (i == index) {
            continue;
        }
        kill(child_processes[i].pid, SIGUSR1);
        child_processes[i].is_stopped = true;
        printf("%s with pid = %d was stopped\n", child_processes[i].name, child_processes[i].pid);
    }

    for (int i = 0; i < 5; ++i) {
        sleep(1);

        char buf[MAX_CHILDREN];
        fflush(stdin);
        fgets(buf, sizeof(buf) / sizeof(*buf), stdin);

        if (buf[0] == 'g') {
            return;
        }

    }
    resume_child_process(-1);
}