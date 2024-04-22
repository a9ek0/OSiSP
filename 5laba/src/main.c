#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include "ring.h"
#include <time.h>

#define BUFFER_SIZE 6

pthread_t *threads = NULL;
size_t thread_count = 0;
pthread_mutex_t mutex;

sem_t *RING_EMPTY;
sem_t *RING_FILLED;
_Thread_local bool IS_RUNNING = true;

void *producer(void *arg);

void *consumer(void *arg);

Message generate_message(void);

void handler_stop_proc();

void display_message(const Message *message);

void initialize_semaphores(void);

void menu(Ring *ring_queue);

void close_semaphores(void);

int main(void) {
    srand(time(NULL));

    signal(SIGUSR1, handler_stop_proc);

    initialize_semaphores();

    Ring *ring_queue = NULL;
    for (size_t i = 0; i < BUFFER_SIZE; ++i)
        allocate_node(&ring_queue);

    printf("Shmid segment : %d\n", ring_queue->shmid);

    menu(ring_queue);

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

    pthread_mutex_init(&mutex, NULL);
}

void menu(Ring *ring_queue) {
    int status;
    char ch;
    do {
        printf("##########################");
        printf("\nSelect an action:\n");
        printf("p - Add a producer\n");
        printf("c - Add a consumer\n");
        printf("+ - Increase queue size\n");
        printf("- - Decrease queue size\n");
        printf("q - Quit\n");
        printf("##########################\n");
        ch = getchar();
        switch (ch) {
            case 'p': {
                pthread_create(&threads[thread_count++], NULL, producer, ring_queue);
                break;
            }
            case 'c': {
                pthread_create(&threads[thread_count++], NULL, consumer, ring_queue);
                break;
            }
            case 'q': {
                IS_RUNNING = false;
                for (size_t i = 0; i < thread_count; ++i) {
                    pthread_join(threads[i], NULL);
                }
                clear_buff(ring_queue);
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

    pthread_mutex_destroy(&mutex);
    sem_close(RING_EMPTY);
    sem_close(RING_FILLED);

    printf("Semaphores closed and unlinked.\n");
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

void *consumer(void *arg) {
    Ring *queue = (Ring *)arg;
    signal(SIGUSR1, handler_stop_proc);
    do {
        sem_wait(RING_FILLED);
        pthread_mutex_lock(&mutex);
        sleep(2);
        Message *message = pop_message(queue);
        pthread_mutex_unlock(&mutex);
        sem_post(RING_EMPTY);
        if (message != NULL) {
            display_message(message);
            free(message);
        }
        printf("Consumed from pthread with id = %lu\n", pthread_self());
        printf("Total messages retrieved = %lu\n", queue->consumed);

        struct timespec req, rem;
        req.tv_sec = 0;
        req.tv_nsec = 20000 * 1000;
        nanosleep(&req, &rem);

    } while (IS_RUNNING);

    return NULL;
}

void *producer(void *arg) {
    Ring *queue = (Ring *)arg;
    signal(SIGUSR1, handler_stop_proc);
    do {
        sem_wait(RING_EMPTY);
        pthread_mutex_lock(&mutex);
        sleep(2);
        Message new_message = generate_message();
        push_message(queue, &new_message);
        pthread_mutex_unlock(&mutex);
        sem_post(RING_FILLED);
        printf("Produced from pthread with id = %lu\n", pthread_self());
        printf("Total objects created = %lu\n", queue->produced);

        struct timespec req, rem;
        req.tv_sec = 0;
        req.tv_nsec = 20000 * 1000;
        nanosleep(&req, &rem);

    } while (IS_RUNNING);

    return NULL;
}