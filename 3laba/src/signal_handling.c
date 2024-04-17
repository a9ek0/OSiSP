#include "signal_handling.h"


extern int kill(pid_t pid, int sig);
extern  int sigemptyset(sigset_t *set);

void init_signals_handling()
{
    struct sigaction action = {0};
    sigset_t set;                              // инициализация сета
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);

    action.sa_flags = SA_SIGINFO;              // sa_sigaction будет принимать 3 параметра
    action.sa_mask = set;      	               // установка запретов на сигналы внутри обработчика, которые могут прийти во время его работы
    action.sa_sigaction = handle_signal;      // установка обработчика сигналов

    sigaction(SIGALRM, &action, NULL);         // установка новых обработчиков сигналов
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
}

void handle_signal(int signo, siginfo_t *info, void *context __attribute__((unused)))
{
    if(signo == SIGUSR1)
    {
        pid_t pid = (*info).si_value.sival_int;
        int index = get_process_index_by_pid(pid);
        if(child_processes[index].is_stopped) kill(pid, SIGUSR1);   // запретить вывод
        else kill(pid, SIGUSR2);                                    // разрешить вывод
    }
    else if(signo == SIGUSR2)
    {
        pid_t pid = (*info).si_value.sival_int;
        char *name = get_process_name_by_pid(pid);
        printf("%s with pid = %d has ended his output\n", name, pid);
    }
    else if(signo == SIGALRM)
        resume_child_process(-1);
}
