#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define NUMBER_COUNT 1224
#define THREAD_COUNT 2

typedef unsigned int index;
typedef unsigned int count;

struct range {
    int *array;
    index start, final;
};

pthread_t threads[THREAD_COUNT];
struct range tasks[THREAD_COUNT];
pthread_mutex_t mutex;  // Mutex pour contrôler les accès concurrents pendant les fusions

void merge(int *array, index start, index middle, index final) {
    index n1 = middle - start + 1;
    index n2 = final - middle;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (index i = 0; i < n1; i++) {
        L[i] = array[start + i];
    }
    for (index j = 0; j < n2; j++) {
        R[j] = array[middle + 1 + j];
    }

    index i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            array[k++] = L[i++];
        } else {
            array[k++] = R[j++];
        }
    }

    while (i < n1) {
        array[k++] = L[i++];
    }
    while (j < n2) {
        array[k++] = R[j++];
    }

    free(L);
    free(R);
}

void *threaded_merge_sort(void *arg) {
    struct range *r = (struct range *)arg;
    if (r->start < r->final) {
        index mid = r->start + (r->final - r->start) / 2;

        struct range left = {r->array, r->start, mid};
        struct range right = {r->array, mid + 1, r->final};

        // Recursive threaded merge sort
        threaded_merge_sort(&left);
        threaded_merge_sort(&right);

        pthread_mutex_lock(&mutex);  // Lock for merging
        merge(r->array, r->start, mid, r->final);
        pthread_mutex_unlock(&mutex);  // Unlock after merging
    }
    return NULL;
}

int main() {
    int numbers[NUMBER_COUNT];
    srand(time(NULL));
    for (index i = 0; i < NUMBER_COUNT; i++) {
        numbers[i] = rand() % 1000;
    }

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    pthread_mutex_init(&mutex, NULL);
    count max_per_thread = ceil((float)NUMBER_COUNT / (float)THREAD_COUNT);

    for (index i = 0; i < THREAD_COUNT; i++) {
        tasks[i].array = numbers;
        tasks[i].start = i * max_per_thread;
        tasks[i].final = (i + 1) * max_per_thread - 1;
        if (i == THREAD_COUNT - 1) {
            tasks[i].final = NUMBER_COUNT - 1;
        }
        pthread_create(&threads[i], NULL, threaded_merge_sort, &tasks[i]);
    }

    for (index i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end_time, NULL);
    long seconds = (end_time.tv_sec - start_time.tv_sec);
    long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
    printf("Temps d'exécution : %ld microsecondes\n", micros);

    for (index i = 0; i < NUMBER_COUNT; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    pthread_mutex_destroy(&mutex);
    return 0;
}
