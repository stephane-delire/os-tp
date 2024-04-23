#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define NUMBER_COUNT 1224
#define THREAD_COUNT 4

typedef unsigned int index;
typedef unsigned int count;

struct range {
    int *array;
    index start, final;
};

pthread_t threads[THREAD_COUNT];
struct range tasks[THREAD_COUNT];

void merge(int array[], index start, index middle, index final) {
    index n1 = middle - start + 1;
    index n2 = final - middle;

    int L[n1], R[n2];

    for (index i = 0; i < n1; i++)
        L[i] = array[start + i];
    for (index j = 0; j < n2; j++)
        R[j] = array[middle + 1 + j];

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
}

void *threaded_merge_sort(void *arg) {
    struct range *r = (struct range *)arg;
    if (r->start < r->final) {
        index mid = r->start + (r->final - r->start) / 2;

        pthread_t left_thread, right_thread;
        struct range left = {r->array, r->start, mid};
        struct range right = {r->array, mid + 1, r->final};

        pthread_create(&left_thread, NULL, threaded_merge_sort, &left);
        pthread_create(&right_thread, NULL, threaded_merge_sort, &right);

        pthread_join(left_thread, NULL);
        pthread_join(right_thread, NULL);

        merge(r->array, r->start, mid, r->final);
    }
    return NULL;
}

int numbers[NUMBER_COUNT];

void fill_array() {
    srand(time(NULL));
    for (index i = 0; i < NUMBER_COUNT; i++) {
        numbers[i] = rand() % 1000;
    }
}

void print_array(int array[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

int main() {
    fill_array();

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    count max_per_thread = ceil((float)NUMBER_COUNT / (float)THREAD_COUNT);

    // Assign each thread a sorting task
    for (index i = 0; i < THREAD_COUNT; i++) {
        tasks[i].array = numbers;
        tasks[i].start = i * max_per_thread;
        tasks[i].final = (i + 1) * max_per_thread - 1;
        if (i == THREAD_COUNT - 1) {
            tasks[i].final = NUMBER_COUNT - 1;
        }
        pthread_create(&threads[i], NULL, threaded_merge_sort, &tasks[i]);
    }

    // Wait for all threads to complete
    for (index i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Now merge the sorted sections
    index segment_length = max_per_thread;
    for (index i = 1; i < THREAD_COUNT; i *= 2) {
        for (index j = 0; j < THREAD_COUNT - i; j += 2*i) {
            index start = j * segment_length;
            index mid = (j + i) * segment_length - 1;
            index end = (j + 2*i) * segment_length - 1;
            if (end >= NUMBER_COUNT) end = NUMBER_COUNT - 1;
            merge(numbers, start, mid, end);
        }
        segment_length *= 2;
    }

    gettimeofday(&end_time, NULL);
    long seconds = (end_time.tv_sec - start_time.tv_sec);
    long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
    printf("Temps d'exécution : %ld microsecondes\n", micros);

    print_array(numbers, NUMBER_COUNT);

    return 0;
}
