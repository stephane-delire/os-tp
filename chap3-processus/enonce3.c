#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>

#define NUMBER_COUNT 1000000
#define THREAD_COUNT 10
#define MAX_DEPTH 0  // Profondeur maximale pour la création de threads
#define MAX_RANDOM 10000000

typedef unsigned int index;
typedef unsigned int count;

struct range {
    int *array;
    index start, final;
    int depth;  // Ajout d'un contrôle de profondeur
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

void merge_sort(int *array, index start, index final) {
    if (start < final) {
        index middle = start + (final - start) / 2;
        merge_sort(array, start, middle);
        merge_sort(array, middle + 1, final);
        merge(array, start, middle, final);
    }
}

void *threaded_merge_sort(void *arg) {
    struct range *r = (struct range *)arg;
    if (r->start < r->final) {
        if (r->depth < MAX_DEPTH) {
            index mid = r->start + (r->final - r->start) / 2;

            struct range left = {r->array, r->start, mid, r->depth + 1};
            struct range right = {r->array, mid + 1, r->final, r->depth + 1};

            pthread_t left_thread, right_thread;
            pthread_create(&left_thread, NULL, threaded_merge_sort, &left);
            pthread_create(&right_thread, NULL, threaded_merge_sort, &right);

            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);

            pthread_mutex_lock(&mutex);  // Lock for merging
            merge(r->array, r->start, mid, r->final);
            pthread_mutex_unlock(&mutex);  // Unlock after merging
        } else {
            merge_sort(r->array, r->start, r->final);
        }
    }
    return NULL;
}

int main() {
    int *numbers = malloc(NUMBER_COUNT * sizeof(int));
    srand(time(NULL));
    for (index i = 0; i < NUMBER_COUNT; i++) {
        numbers[i] = rand() % MAX_RANDOM;
    }

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    pthread_mutex_init(&mutex, NULL);
    struct range main_range = {numbers, 0, NUMBER_COUNT - 1, 0};
    threaded_merge_sort(&main_range);

    gettimeofday(&end_time, NULL);
    long seconds = (end_time.tv_sec - start_time.tv_sec);
    long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
    printf("Temps d'exécution : %ld microsecondes\n", micros);

    for (index i = 0; i < NUMBER_COUNT; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");
    printf("Temps d'exécution : %ld microsecondes\n", micros);
    pthread_mutex_destroy(&mutex);
    free(numbers);
    return 0;
}
