#include <stdio.h>
#include <stdbool.h>
#include "process_management.h"
#include "signal_handling.h"
#include "utilities.h"

int main(void) {
    init_signals_handling();
    allocate_child_processes();

    while (true) {
        char option;
        int option_index;

        fflush(stdin);

        if (parse_input_option(&option, &option_index) == -1)
            continue;

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
                remove_all_child_processes();
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
}

