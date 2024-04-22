#include "ring.h"

node *create_node() {
    int32_t shmid = shmget(0, sizeof(node), 0777);
    node *buffer = shmat(shmid, NULL, 0);
    buffer->shmid_curr = shmid;
    buffer->shmid_next = shmid;
    buffer->shmid_prev = shmid;
    buffer->flag_is_busy = false;
    return buffer;
}

ring_shared_buffer *create_buff() {
    int32_t shmid = shmget(0, sizeof(ring_shared_buffer), 0777);
    ring_shared_buffer *buffer = shmat(shmid, NULL, 0);
    buffer->shmid_tail = 0;
    buffer->shmid_begin = 0;
    buffer->consumed = 0;
    buffer->produced = 0;
    buffer->shmid = shmid;
    return buffer;
}

void append(ring_shared_buffer **begin) {
    if (begin == NULL)
        exit(-100);
    if (*begin == NULL) {
        *begin = create_buff();
    }
    node *buffer = create_node();
    if ((*begin)->shmid_begin == 0) {
        (*begin)->shmid_begin = buffer->shmid_curr;
        (*begin)->shmid_tail = buffer->shmid_curr;
        return;
    }
    node *curr = shmat((*begin)->shmid_begin, NULL, 0);
    if (curr->shmid_curr == curr->shmid_next) {
        buffer->shmid_next = buffer->shmid_prev = curr->shmid_curr;
        curr->shmid_next = curr->shmid_prev = buffer->shmid_curr;
        return;
    }
    node *prev = shmat(curr->shmid_prev, NULL, 0);
    buffer->shmid_next = curr->shmid_curr;
    buffer->shmid_prev = prev->shmid_curr;
    prev->shmid_next = buffer->shmid_curr;
    curr->shmid_prev = buffer->shmid_curr;
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
    node *curr = shmat(ring->shmid_tail, NULL, 0);
    if (curr->flag_is_busy == true) {
        printf("No free places.\n");
        return;
    }

    *curr->message = *message;

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
    node *curr = shmat(ring->shmid_begin, NULL, 0);
    if (curr->flag_is_busy == false) {
        printf("No messages to retrieve.\n");
        return NULL;
    }

    curr->flag_is_busy = false;
    Message *message = (Message *) calloc(1, sizeof(Message));

    if (curr->message[0].size > 0) { // Check if the message has data
        memcpy(message->data, curr->message[0].data, curr->message[0].size);
        message->size = curr->message[0].size;
        message->hash = curr->message[0].hash;
        message->type = curr->message[0].type;
    }

    ring->shmid_begin = curr->shmid_next;
    ring->consumed++;
    return message;
}

void clear_buff(ring_shared_buffer *ring_queue) {
    int32_t curr;
    node *buffer = shmat(ring_queue->shmid_begin, NULL, 0);
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