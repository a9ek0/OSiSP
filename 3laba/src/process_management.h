#ifndef PROCESS_MANAGEMENT_H
#define PROCESS_MANAGEMENT_H

#include "globals.h"

void allocate_child_processes();
void create_child_process();
void terminate_last_child_process();
void list_all_processes();
void terminate_all_child_processes();
void stop_child_process(int index);
void resume_child_process(int index);
void prioritize_child_process(int index);
void quit_program();
void child_init (pid_t pid);
process_info get_last_child_process();
void remove_child_process();
void remove_all_child_processes();
char *get_process_name_by_pid(pid_t pid);
int get_process_index_by_pid(pid_t pid);
void delete_child_process(pid_t pid);

#endif