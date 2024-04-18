#define _POSIX_C_SOURCE 199309L

#include "signal_handling.h"

void init_signals_handling(void) {
    struct sigaction action = {0};
    sigset_t set;
    sigemptyset(&set);

    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    action.sa_flags = SA_SIGINFO;
    action.sa_mask = set;
    action.sa_sigaction = handle_signal;

    sigaction(SIGALRM, &action, NULL);
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
}

void handle_signal(int signo, siginfo_t *info, void *context __attribute__((unused))) {
    if (signo == SIGUSR1) {
        pid_t pid = (*info).si_value.sival_int;
        int index = get_process_index_by_pid(pid);
        if (child_processes[index].is_stopped) {
            kill(pid, SIGUSR1);
        } else {
            kill(pid, SIGUSR2);
        }
    } else if (signo == SIGUSR2) {
        pid_t pid = (*info).si_value.sival_int;
        char *name = get_process_name_by_pid(pid);
        printf("%s with pid = %d has ended his output\n", name, pid);
    } else if (signo == SIGALRM) {
        resume_child_process(-1);
    }
}

void update_stats(void) {
    static int counter = 0;
    stats.first = counter / 2 % 2;
    stats.second = counter % 2;
    counter = (counter + 1) % 4;
}

void init_signals_handling_child(void) {
    setup_signal_handler(SIGUSR1, user_signal_handler, 0);
    setup_signal_handler(SIGUSR2, user_signal_handler, 0);
    setup_signal_handler(SIGALRM, alarm_signal_handler, 0);
}

void setup_signal_handler(int signo, void (*handler)(int), int flags) {
    struct sigaction action = {0};
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, signo);
    action.sa_flags = flags;
    action.sa_mask = set;
    action.sa_handler = handler;
    sigaction(signo, &action, NULL);
}

void user_signal_handler(int signo) {
    if (signo == SIGUSR1) {
        state = PRINT_FORBIDDEN;
        received_signal = true;
    } else if (signo == SIGUSR2) {
        state = PRINT_ALLOWED;
        received_signal = true;
    }
}

void alarm_signal_handler(int signo __attribute__((unused))) {
    if (stats.first == 0 && stats.second == 0) {
        c00++;
    } else if (stats.first == 1 && stats.second == 0) {
        c01++;
    } else if (stats.first == 0 && stats.second == 1) {
        c10++;
    } else if (stats.first == 1 && stats.second == 1) {
        c11++;
    }

    alarm(rand() % 1 + 1);
}
