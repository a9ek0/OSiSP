#ifndef RING_H
#define RING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>

#define LEN_MESSAGE 255

typedef struct {
    u_int8_t data[LEN_MESSAGE];
    u_int16_t hash;
    u_int8_t size;
    u_int8_t type;
} Message;

typedef struct node_ring {
    int32_t shmid_curr;
    int32_t shmid_next;
    int32_t shmid_prev;
    Message message[LEN_MESSAGE];
    bool flag_is_busy;
} node;

typedef struct ring_shared_buffer {
    int32_t shmid;
    size_t consumed;
    size_t produced;
    int32_t shmid_begin;
    int32_t shmid_tail;
} ring_shared_buffer;

ring_shared_buffer *create_buff();

void clear_buff(ring_shared_buffer *);

Message *extract_message(ring_shared_buffer *);

node *create_node();

void append(ring_shared_buffer **);

void add_message(ring_shared_buffer *, Message *);

#endif //RING_H

