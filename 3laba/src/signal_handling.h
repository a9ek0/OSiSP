#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H
#define _POSIX_C_SOURCE 199309L

#include "process_management.h"
#include "stdio.h"
#include "globals.h"
#include <signal.h>
#include <bits/types/siginfo_t.h>

void init_signals_handling();
void handle_signal(int signo, siginfo_t *info, void *context);

#endif