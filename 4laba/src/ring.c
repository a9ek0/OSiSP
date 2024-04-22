#include "ring.h"

node_ring *constructor_node() {
    int32_t shmid = shmget(0, sizeof(node_ring), 0777);
    node_ring *buffer = shmat(shmid, NULL, 0);
    buffer->shmid_curr = shmid;
    buffer->shmid_next = shmid;
    buffer->shmid_prev = shmid;
    buffer->flag_is_busy = false;
    return buffer;
}

ring_shared_buffer *constructor_buffer() {
    int32_t shmid = shmget(0, sizeof(ring_shared_buffer), 0777);
    ring_shared_buffer *buffer = shmat(shmid, NULL, 0);
    buffer->shmid_tail = 0;
    buffer->shmid_begin = 0;
    buffer->consumed = 0;
    buffer->produced = 0;
    buffer->shmid = shmid;
    return buffer;
}

void append(ring_shared_buffer **__begin) {
    if (__begin == NULL)
        exit(-100);
    if (*__begin == NULL) {
        *__begin = constructor_buffer();
    }
    node_ring *__buffer = constructor_node();
    if ((*__begin)->shmid_begin == 0) {
        (*__begin)->shmid_begin = __buffer->shmid_curr;
        (*__begin)->shmid_tail = __buffer->shmid_curr;
        return;
    }
    node_ring *__curr = shmat((*__begin)->shmid_begin, NULL, 0);
    if (__curr->shmid_curr == __curr->shmid_next) {
        __buffer->shmid_next = __buffer->shmid_prev = __curr->shmid_curr;
        __curr->shmid_next = __curr->shmid_prev = __buffer->shmid_curr;
        return;
    }
    node_ring *__prev = shmat(__curr->shmid_prev, NULL, 0);
    __buffer->shmid_next = __curr->shmid_curr;
    __buffer->shmid_prev = __prev->shmid_curr;
    __prev->shmid_next = __buffer->shmid_curr;
    __curr->shmid_prev = __buffer->shmid_curr;
}

void add_message(ring_shared_buffer *ring, Message *message) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return;
    }
    if (ring->shmid_begin == 0) {
        printf("There are 0 places in the ring.\n");
        return;
    }
    node_ring *curr = shmat(ring->shmid_tail, NULL, 0);
    if (curr->flag_is_busy == true) {
        printf("No free places.\n");
        return;
    }

    *curr->message = *message; // Directly copy the message structure.

    curr->flag_is_busy = true;
    ring->shmid_tail = curr->shmid_next;
    ring->produced++;
}

Message *extract_message(ring_shared_buffer *ring) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return NULL;
    }
    if (ring->shmid_begin == 0) {
        printf("There are 0 places in the ring.\n");
        return NULL;
    }
    node_ring *curr = shmat(ring->shmid_begin, NULL, 0);
    if (curr->flag_is_busy == false) {
        printf("No messages to retrieve.\n");
        return NULL;
    }

    curr->flag_is_busy = false;
    Message *message = (Message *) calloc(1, sizeof(Message)); // Allocate memory for a Message, not u_int8_t array

    // Assuming you want to copy the data from the first message in the node_ring's message array
    if (curr->message[0].size > 0) { // Check if the message has data
        memcpy(message->data, curr->message[0].data, curr->message[0].size);
        message->size = curr->message[0].size;
        message->hash = curr->message[0].hash;
        message->type = curr->message[0].type; // Assuming you also want to copy the type
    }

    ring->shmid_begin = curr->shmid_next;
    ring->consumed++;
    return message;
}

void clear_shared_memory(ring_shared_buffer *ring_queue) {
    int32_t curr;
    node_ring *buffer = shmat(ring_queue->shmid_begin, NULL, 0);
    while (buffer->shmid_next != ring_queue->shmid_tail) {
        curr = buffer->shmid_curr;
        int32_t shmid_next = buffer->shmid_next;
        shmdt(buffer);
        shmctl(curr, IPC_RMID, NULL);
        buffer = shmat(shmid_next, NULL, 0);
    }
    curr = buffer->shmid_curr;
    shmdt(buffer);
    shmctl(curr, IPC_RMID, NULL);
    curr = ring_queue->shmid;
    shmdt(ring_queue);
    shmctl(curr, IPC_RMID, NULL);
}