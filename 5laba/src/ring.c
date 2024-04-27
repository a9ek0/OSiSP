#include "ring.h"

#include <inttypes.h>

Ring_node *create_node() {
    Ring_node *buffer = (Ring_node *) calloc(1, sizeof(Ring_node));
    if (!buffer) {
        perror("Failed to allocate memory for node");
        exit(EXIT_FAILURE);
    }
    buffer->next = NULL;
    buffer->prev = NULL;
    buffer->is_used = false;
    // Assuming Message structure has a default initialization method or is zero-initialized
    memset(&buffer->message, 0, sizeof(Message));
    return buffer;
}

Ring *init_ring() {
    Ring *buffer = (Ring *) malloc(sizeof(Ring));
    if (!buffer) {
        perror("Failed to allocate memory for ring");
        exit(EXIT_FAILURE);
    }
    buffer->begin = NULL;
    buffer->tail = NULL;
    buffer->size_queue = 0;
    buffer->produced = 0;
    buffer->consumed = 0;
    return buffer;
}

void append(Ring **ring, bool flag_after) {
    // Проверка на NULL указатель на кольцо
    if (ring == NULL)
        exit(-100);
    // Если кольцо не инициализировано, инициализируем его
    if (*ring == NULL) {
        *ring = init_ring(); // Инициализация кольца
        Ring_node *new_node = create_node(); // Создание нового узла
        // Установка нового узла как начального и конечного элемента кольца
        (*ring)->begin = (*ring)->tail = new_node;
        // Установка связей нового узла на самого себя, т.к. в кольце только один узел
        new_node->next = new_node->prev = new_node;
        (*ring)->size_queue++; // Увеличение размера кольца
        return;
    }
    // Увеличение размера кольца
    (*ring)->size_queue++;
    // Создание нового узла
    Ring_node *buffer = create_node();
    // Если в кольце только один узел
    if ((*ring)->begin->next == (*ring)->begin) {
        // Установка нового узла между существующим узлом и самим собой
        (*ring)->begin->next = (*ring)->begin->prev = buffer;
        buffer->next = buffer->prev = (*ring)->begin;
    } else {
        // Вставка нового узла перед начальным узлом кольца
        buffer->next = (*ring)->begin;
        buffer->prev = (*ring)->begin->prev;
        buffer->prev->next = buffer;
        (*ring)->begin->prev = buffer;
    }
    // Если флаг flag_after установлен и новый узел находится перед хвостом кольца
    if (flag_after) {
        if (buffer->next == (*ring)->tail) {
            // Если хвост и начало кольца совпадают и начальный узел не используется,
            // устанавливаем новый узел как начало и конец кольца
            if ((*ring)->tail == (*ring)->begin && !(*ring)->begin->is_used) {
                (*ring)->tail = (*ring)->begin = buffer;
            } else {
                // Иначе, просто перемещаем хвост кольца на новый узел
                (*ring)->tail = buffer;
            }
        }
    }
}

bool erase(Ring **ring) {
    // Проверка на NULL указатели и наличие элементов в кольце
    if (ring == NULL || *ring == NULL || (*ring)->begin == NULL) {
        fprintf(stderr, "The queue is empty or not initialized.\n");
        exit(-100);
    }
    // Уменьшаем размер кольца на один
    (*ring)->size_queue--;
    bool result = false; // Результат, указывающий, был ли узел использован

    // Выбираем узел для удаления, начиная с хвоста
    Ring_node *to_erase = (*ring)->tail;

//    if ((*ring)->begin == (*ring)->tail) { // Если в кольце только один элемент
    if ((*ring)->size_queue == 0) { // Если в кольце только один элемент
        printf("Only one element left in the ring, cannot erase.\n");
        result = to_erase->is_used;
        (*ring)->size_queue++;
    } else { // Если в кольце несколько элементов
        // Перестраиваем связи между узлами, исключая удаляемый узел из кольца
        to_erase->prev->next = to_erase->next;
        to_erase->next->prev = to_erase->prev;
        // Если удаляемый узел является началом кольца, перемещаем начало
        if (to_erase == (*ring)->begin) {
            (*ring)->begin = to_erase->next;
        }
        // Если удаляемый узел является концом кольца, перемещаем конец
        if (to_erase == (*ring)->tail) {
            (*ring)->tail = to_erase->prev;
        }
        result = to_erase->is_used; // Запоминаем, был ли узел использован
        free(to_erase); // Освобождаем память узла
    }

    return result; // Возвращаем результат, указывающий, был ли узел использован
}

void clear_ring(Ring **ring) {
    if (ring == NULL || *ring == NULL)
        return;
    Ring_node *current = (*ring)->begin;
    Ring_node *next;
    for (size_t i = 0; i < (*ring)->size_queue; ++i) {
        next = current->next;
        free(current);
        current = next;
    }
    free(*ring);
    *ring = NULL;
}

void push_message(Ring *ring, Message *message) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return;
    }
    if (ring->begin == NULL) {
        printf("There are 0 places in the ring.\n");
        return;
    }
    Ring_node *curr = ring->tail;
    if (curr->is_used == true) {
        printf("No free places.\n");
        return;
    }
    // Correctly copy the message to the current node
    curr->message = *message; // Directly assign the message struct
    curr->is_used = true;
    ring->tail = ring->tail->next;
    ring->produced++;
}

Message *pop_message(Ring *ring) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return NULL;
    }
    if (ring->begin == NULL) {
        printf("There are 0 places in the ring.\n");
        return NULL;
    }
    Ring_node *curr = ring->begin;
    if (curr->is_used == false) {
        printf("No messages to pop.\n");
        return NULL;
    }

    // Allocate memory for a new Message struct and copy the message
    Message *popped_message = (Message *) malloc(sizeof(Message));
    if (!popped_message) {
        perror("Failed to allocate memory for message");
        exit(EXIT_FAILURE);
    }
    *popped_message = curr->message; // Copy the message struct

    curr->is_used = false;
    ring->begin = ring->begin->next;
    ring->consumed++;
    return popped_message;
}

void print_ring_nodes(const Ring *ring) {
    if (ring == NULL || ring->begin == NULL) {
        printf("The ring is empty.\n");
        return;
    }
    Ring_node *current = ring->begin;
    int count = 1;
    do {
        printf("Node %d\n", count++);
        current = current->next;
    } while (current != ring->begin); // Продолжаем, пока не вернемся к начальному узлу
}
