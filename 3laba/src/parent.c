#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "process_management.h"
#include "signal_handling.h"
#include "utilities.h"
#include "globals.h"

size_t num_child_processes = 0;
size_t max_child_processes = MAX_CHILDREN;
process_info *child_processes = NULL;
char child_name[CHILD_NAME_LENGTH] = "./child";

int main() {
    init_signals_handling();
    allocate_child_processes();
    while (true) {
        char option;
        int option_index;
        fflush(stdin);
        if (parse_input_option(&option, &option_index) == -1) continue;
        switch (option) {
            case '+': {
                create_child_process();
                break;
            }
            case '-': {
                terminate_last_child_process();
                break;
            }
            case 'l': {
                list_all_processes();
                break;
            }
            case 'k': {
                terminate_all_child_processes();
                break;
            }
            case 's': {
                stop_child_process(option_index);
                break;
            }
            case 'g': {
                resume_child_process(option_index);
                break;
            }
            case 'p': {
                prioritize_child_process(option_index);
                break;
            }
            case 'q': {
                quit_program();
            }
        }
    }
    return 0;
}

