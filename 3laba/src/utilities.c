#include "utilities.h"
#include "globals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_input_option(char *option, int *option_index) {
    int tmp_index = -1;
    char buf[MAX_CHILDREN];
    fflush(stdin);
    fgets(buf, sizeof(buf) / sizeof(*buf), stdin);
    char tmp_option = buf[0];

    if ((strstr(buf, "<") && strstr(buf, ">")) && (tmp_option == 's' || tmp_option == 'g' || tmp_option == 'p')) {
        tmp_index = str_to_int(buf);
        if (tmp_index == -1) {
            fprintf(stderr, "Error, input string in format: o<num>\no - option, num - index of process\n");
            return -1;
        }

        if (tmp_index >= (int) num_child_processes) {
            fprintf(stderr, "Error, index %d is greater or equal than num_child_processes =% d\n", tmp_index,
                    (int) num_child_processes);
            return -1;
        }
    }

    if (!strchr("+-mlksgpq", tmp_option)) {
        return -1;
    }

    if (tmp_index == -1 && num_child_processes == 0 &&
        (tmp_option != '+' && tmp_option != 'q' && tmp_option != 'l' && tmp_option != 'm')) {
        fprintf(stderr, "Error, there are no child_processes\ncreate them with + option\n");
        return -1;
    }

    if (tmp_index == -1 && tmp_option == 'p') {
        fprintf(stderr, "Error, option p can be only chosen with num\n");
        return -1;
    }

    *option = tmp_option;
    *option_index = tmp_index;

    return 0;
}

int str_to_int(char *str) {
    int length = 0;
    int str_len = strlen(str);
    char num[MAX_CHILDREN];

    for (int i = 0; i < str_len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            length++;
            num[length - 1] = str[i];
        }
    }

    if (length && num[0] == '0')
        return 0;
    else if (length)
        return atoi(num);

    return -1;
}
