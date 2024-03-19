// Vraie liste circulaire
// Crée a partir de la réponse de l'exo6

#include <stdio.h>
#include <stdlib.h>

#define QUEUE_SIZE 5 // Taille fixée à 5 pour maximum 5 éléments dans la liste circulaire

struct circularQueue {
    int items[QUEUE_SIZE];
    int head;
    int tail;
    int size;
};

// Crée une nouvelle file circulaire
struct circularQueue* createQueue() {
    struct circularQueue* q = malloc(sizeof(struct circularQueue));
    q->head = -1;
    q->tail = -1;
    q->size = 0;
    return q;
}

// Vérifie si la file est pleine
int isFull(struct circularQueue* q) {
    return q->size == QUEUE_SIZE;
}

// Vérifie si la file est vide
int isEmpty(struct circularQueue* q) {
    return q->size == 0;
}

// Ajoute un élément à la queue
void enqueue(struct circularQueue* q, int element) {
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    if (isEmpty(q)) {
        q->head = 0;
    }
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->items[q->tail] = element;
    q->size++;
}

// Retire et retourne l'élément en tête de la queue
int dequeue(struct circularQueue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1;
    }
    int item = q->items[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->size--;
    if (isEmpty(q)) {
        q->head = -1;
        q->tail = -1;
    }
    return item;
}

// Affiche la queue
void printQueue(struct circularQueue* q) {
    printf("Queue: ");
    if (isEmpty(q)) {
        printf("is empty\n");
        return;
    }
    int i = q->head;
    do {
        printf("%d ", q->items[i]);
        i = (i + 1) % QUEUE_SIZE;
    } while (i != (q->tail + 1) % QUEUE_SIZE);
    printf("\n");
}

int main() {
    struct circularQueue* q = createQueue();

    enqueue(q, 1);
    enqueue(q, 2);
    enqueue(q, 3);
    enqueue(q, 4);
    enqueue(q, 5);

    printQueue(q);

    printf("Dequeued: %d\n", dequeue(q));
    printf("Dequeued: %d\n", dequeue(q));

    printQueue(q);

    enqueue(q, 6);
    enqueue(q, 7);

    printQueue(q);

    free(q);

    return 0;
}
