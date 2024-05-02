#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

#define _POSIX_C_SOURCE 199309L
#include "globals.h"

void allocate_child_processes(void);

void create_child_process(void);

void terminate_last_child_process(void);

void list_all_processes(void);

void stop_child_process(int index);

void resume_child_process(int index);

void prioritize_child_process(int index);

void quit_program(void);

process_info get_last_child_process(void);

void remove_all_child_processes(void);

char *get_process_name_by_pid(pid_t pid);

int get_process_index_by_pid(pid_t pid);

#endif