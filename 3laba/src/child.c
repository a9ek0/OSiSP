#define _GNU_SOURCE

#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    int first;
    int second;
} pair;

typedef enum {
    WAITING,
    PRINT_ALLOWED,
    PRINT_FORBIDDEN
} child_state;

void init_signals_handling();

void user_signal_handler(int signo);

void alarm_signal_handler(int signo);

void update_stats();

void setup_signal_handler(int signo, void (*handler)(int), int flags);

bool received_signal = false;
pair stats;
size_t c00 = 0, c01 = 0, c10 = 0, c11 = 0;
child_state state = WAITING;

int main() {
    srand(time(NULL));
    init_signals_handling();
    alarm(rand() % 1 + 1); // Use a wider range for randomness

    for (int i = 0;; i++) {
        sleep(1);
        update_stats();
        received_signal = false;
        if (i >= 5 && state == PRINT_ALLOWED) {
            alarm(0); // Disable alarm
            union sigval info;
            info.sival_int = getpid();
            received_signal = false;
            while (!received_signal) {
                sigqueue(getppid(), SIGUSR1, info);
                sleep(10);
            }
            alarm(rand() % 1 + 1); // Use a wider range for randomness
            if (state == PRINT_FORBIDDEN) {
                i = 0;
                state = PRINT_ALLOWED;
                continue;
            }
            printf("------------------------------------------------\n");
            printf("Parent PID: %5d | Child PID: %5d\n", (int) getppid(), (int) getpid());
            printf("Pair Counts: 00: %5d | 01: %5d | 10: %5d | 11: %5d\n", (int) c00, (int) c01, (int) c10, (int) c11);
            i = 0;
            sigqueue(getppid(), SIGUSR2, info);
        }
    }
    return 0;
}

void update_stats() {
    static int counter = 0;
    stats.first = counter / 2 % 2;
    stats.second = counter % 2;
    counter = (counter + 1) % 4;
}

void init_signals_handling() {
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
    if (stats.first == 0 && stats.second == 0) c00++;
    else if (stats.first == 1 && stats.second == 0) c01++;
    else if (stats.first == 0 && stats.second == 1) c10++;
    else if (stats.first == 1 && stats.second == 1) c11++;
    alarm(rand() % 1 + 1); // Use a wider range for randomness
}