#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <sys/types.h>

#define MAX_CHILDREN 8
#define CHILD_NAME_LENGTH 16
#define SEPARATE "################################################\n"

typedef struct process_info {
    pid_t pid;
    bool is_stopped;
    char name[CHILD_NAME_LENGTH];
} process_info;

typedef struct {
    int first;
    int second;
} pair;

typedef enum {
    WAITING,
    PRINT_ALLOWED,
    PRINT_FORBIDDEN
} child_state;

extern bool received_signal;
extern pair stats;
extern size_t c00, c01, c10, c11;
extern child_state state;
extern size_t num_child_processes;
extern size_t max_child_processes;
extern process_info *child_processes;
extern char child_name[CHILD_NAME_LENGTH];

#endif