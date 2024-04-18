#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H
#define _POSIX_C_SOURCE 199309L

#include "process_management.h"
#include "stdio.h"
#include "globals.h"
#include <signal.h>
#include <time.h>
#include <bits/types/siginfo_t.h>
#include <stdlib.h>
#include <unistd.h>

void init_signals_handling(void);

void handle_signal(int signo, siginfo_t *info, void *context);

void init_signals_handling_child(void);

void user_signal_handler(int signo);

void alarm_signal_handler(int signo);

void update_stats(void);

void setup_signal_handler(int signo, void (*handler)(int), int flags);

#endif