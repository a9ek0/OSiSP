#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "ring.h"
#include <time.h>

#define BUFFER_SIZE 6
#define SIGKILL     9
#define SIGUSR1     10

pid_t *childs = NULL;
size_t total_size = 0;

sem_t *RING_EMPTY;
sem_t *RING_FILLED;
sem_t *MUTEX;
volatile bool IS_RUNNING = true;

u_int16_t control_sum(const u_int8_t *, size_t);

void producer(int32_t);

void consumer(int32_t);

Message generate_message(void);

void handler_stop_proc();

void display_message(const Message *message);

void initialize_semaphores(void);

void handle_menu(ring_shared_buffer *ring_queue);

void close_semaphores(void);

int main(void) {
    srand(time(NULL));
    signal(SIGUSR1, handler_stop_proc);

    initialize_semaphores();

    ring_shared_buffer *ring_queue = NULL;

    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        append(&ring_queue);

    printf("Shmid segment : %d\n", ring_queue->shmid);

    handle_menu(ring_queue);

    close_semaphores();
    return 0;
}

void initialize_semaphores(void) {
    sem_unlink("RING_FILLED");
    sem_unlink("RING_EMPTY");
    sem_unlink("MUTEX");

    RING_FILLED = sem_open("RING_FILLED", O_CREAT, 0777, 0);
    if (RING_FILLED == SEM_FAILED) {
        perror("Failed to open RING_FILLED semaphore");
        exit(EXIT_FAILURE);
    }

    RING_EMPTY = sem_open("RING_EMPTY", O_CREAT, 0777, BUFFER_SIZE);
    if (RING_EMPTY == SEM_FAILED) {
        perror("Failed to open RING_EMPTY semaphore");
        exit(EXIT_FAILURE);
    }

    MUTEX = sem_open("MUTEX", O_CREAT, 0777, 1);
    if (MUTEX == SEM_FAILED) {
        perror("Failed to open MUTEX semaphore");
        exit(EXIT_FAILURE);
    }
}

void handle_menu(ring_shared_buffer *ring_queue) {
    int status;
    char ch;
    do {
        printf("##########################");
        printf("\nSelect an action:\n");
        printf("p - Add a producer\n");
        printf("c - Add a consumer\n");
        printf("l - List processes\n");
        printf("q - Quit\n");
        printf("##########################\n");
        ch = getchar();
        switch (ch) {
            case 'p': {
                pid_t pid = fork();
                if (pid == 0) {
                    producer(ring_queue->shmid);
                } else {
                    childs = (pid_t *) realloc(childs, (total_size + 1) * sizeof(pid_t));
                    childs[total_size++] = pid;
                }
                break;
            }
            case 'c': {
                pid_t pid = fork();
                if (pid == 0) {
                    consumer(ring_queue->shmid);
                } else {
                    childs = (pid_t *) realloc(childs, (total_size + 1) * sizeof(pid_t));
                    childs[total_size++] = pid;
                }
                break;
            }
            case 'q': {
                for (size_t i = 0; i < total_size; ++i) {
                    kill(childs[i], SIGUSR1);
                    kill(childs[i], SIGKILL);
                }
                clear_buff(ring_queue);
                IS_RUNNING = false;
                break;
            }
            default: {
                printf("Incorrect input.\n");
                fflush(stdin);
                break;
            }
        }
        waitpid(-1, &status, WNOHANG);
        getchar();
    } while (IS_RUNNING);
}

void close_semaphores(void) {
    sem_unlink("RING_FILLED");
    sem_unlink("RING_EMPTY");
    sem_unlink("MUTEX");

    sem_close(MUTEX);
    sem_close(RING_EMPTY);
    sem_close(RING_FILLED);

    printf("Semaphores closed and unlinked.\n");
}


u_int16_t control_sum(const u_int8_t *data, size_t length) {
    u_int16_t hash = 0;
    for (size_t i = 0; i < length; ++i) {
        hash += data[i];
    }
    return hash;
}

Message generate_message(void) {
    Message message = {
            .data = {0},
            .hash = 0,
            .size = 0,
            .type = 0
    };

    do {
        message.size = rand() % 257;
    } while (message.size == 0);

    size_t realSize = message.size;
    if (realSize == 256) {
        message.size = 0;
        realSize = (message.size == 0) ? 256 : message.size;
    }

    message.hash = 0;
    for (size_t i = 0; i < realSize; ++i) {
        message.data[i] = rand() % 256;
        message.hash += message.data[i];
    }

    return message;
}

void display_message(const Message *message) {
    for (int i = 0; i < message->size; ++i) {
        printf("%02X", message->data[i]);
    }
    printf("\n");
}

void handler_stop_proc() {
    IS_RUNNING = false;
}

void consumer(int32_t shmid) {
    ring_shared_buffer *queue = shmat(shmid, NULL, 0);
    do {
        sem_wait(RING_FILLED);
        sem_wait(MUTEX);
        sleep(2);
        Message *message = extract_message(queue);
        if (message != NULL) {
            display_message(message);
            free(message);
        }
        sem_post(MUTEX);
        sem_post(RING_EMPTY);
        printf("Consumed from CHILD with PID = %d\n", getpid());
        printf("Total messages retrieved = %lu\n", queue->consumed);
    } while (IS_RUNNING);
    shmdt(queue);
}

void producer(int32_t shmid) {
    ring_shared_buffer *queue = shmat(shmid, NULL, 0);
    do {
        sem_wait(RING_EMPTY);
        sem_wait(MUTEX);
        sleep(2);
        Message new_message = generate_message();
        add_message(queue, &new_message);
        sem_post(MUTEX);
        sem_post(RING_FILLED);
        printf("Produced from CHILD with PID = %d\n", getpid());
        printf("Total objects created = %lu\n", queue->produced);
    } while (IS_RUNNING);
    shmdt(queue);
}