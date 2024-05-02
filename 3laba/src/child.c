#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "signal_handling.h"
#include "globals.h"

int main(void) {
    srand(time(NULL));

    init_signals_handling_child();

    alarm(rand() % 1 + 1); // Установка таймера для отправки сигнала SIGALRM через случайное время

    for (int i = 0;; i++) {
        sleep(1);
        update_stats();
        received_signal = false;

        if (i >= 5 && state == PRINT_ALLOWED) { // Прошло 5 секуннд
            alarm(0); // Отмена текущего таймера SIGALRM

            union sigval info; // Объединение для передачи данных с сигналом
            info.sival_int = getpid();
            received_signal = false;

            while (!received_signal) {
                sigqueue(getppid(), SIGUSR1, info); // Отправка сигнала SIGUSR1 родительскому процессу с PID текущего процесса
                sleep(10);
            }

            alarm(rand() % 1 + 1);

            if (state == PRINT_FORBIDDEN) {
                i = 0;
                state = PRINT_ALLOWED;
                continue;
            }

            printf(SEPARATE);
            printf("Parent PID: %5d | Child PID: %5d\n", (int) getppid(), (int) getpid());
            printf("Pair Counts: 00: %5d | 01: %5d | 10: %5d | 11: %5d\n", (int) c00, (int) c01, (int) c10, (int) c11);
            printf(SEPARATE);

            i = 0;

            sigqueue(getppid(), SIGUSR2, info); // Отправка сигнала SIGUSR2 родительскому процессу
        }
    }
    return 0;
}