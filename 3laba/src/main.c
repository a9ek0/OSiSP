#include "mainFunctions.h"

NODE* head = NULL;

int main() {
    signal(SIGINT, printStat);                                                  // Сигнал вывода информации.
    signal(SIGUSR1, allow);                                                     // Сигнал разрешения вывода данных.
    signal(SIGUSR2, forbid);                                                    // Сигнал запрещения вывода данных.

    int cont = 1;
    while(cont) {
        char ch = getchar(), n;
        int num = -1;
        while((n=getchar())!='\n') {
            if (num == -1) num++;
            num *= 10;
            num += n - '0';
        }
        switch(ch) {
            case '+':
                makeFork();                                                    // Создать процесс.
                break;
            case '-':
                delLastFork();                                                 // Удалить процесс.
                break;
            case 'l':
                printf("Process P with pid = %d\n", getpid());                 // Вывести все процессы.
                show(head);
                break;
            case 'k':
                while(head)
                    delLastFork();                                             // Удалить все.
                break;
            case 's':
                sendForbid(num);                                               // Запретить.
                break;
            case 'g':
                sendAllow(num);                                                // Разрешить.
                break;
            case 'p':
                forbidAndShow(num);                                            // Вывести данные одним процессом, запретив другим.
                break;
            case 'q':
                while(head)
                    delLastFork();                                             // Конец.
                printf("All processes are deleted. Program ends.\n");
                cont = 0;
                break;
        }
    }

    return 0;
}