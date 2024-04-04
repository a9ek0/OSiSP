#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define REPEAT_COUNT 101

typedef struct {
    int firstVal;
    int secondVal;
} PAIR;

extern NODE* head;

void makeFork();           // Создать процесс.
void delLastFork();        // Удалить последний созданный.
void sendForbid(int);      // Сигнал запрещение.
void sendAllow(int);       // Сигнал разрешение.
void forbid();             
void allow();
void forbidAndShow(int);   // Вывести данные одного.

void printStat();          // Вывод статистики.
void cycle();              // Цикл.
void takeStatistics();     // Сбор статистики.
void allowAfterP();        // Разрешение вывода всем.