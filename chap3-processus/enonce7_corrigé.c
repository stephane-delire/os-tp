#include <stdio.h>       // printf()
#include <unistd.h>      // fork(), getpid()
#include <stdlib.h>      // EXIT_SUCCESS, rand(), srand()
#include <sys/wait.h>    // waitpid()
#include <time.h>        // time()
#include <math.h>        // ceil()
#include <pthread.h>     // pthread_t, pthread_create(), etc.

#define NUMBER_COUNT 1224
#define THREAD_COUNT 10

// Calculate starting index for each thread
#define START(id) ((id) * max_per_thread)

typedef unsigned int index;
typedef unsigned int count;

struct range {
    unsigned long *array;   // Array of numbers to use
    index start, final;     // Indices for beginning and end of range (chosen by main thread)
    unsigned long total;    // Sum of elements in range (computed by worker thread)
};

// Used for slicing the complete array into roughly equal parts
count max_per_thread = ceil((float)NUMBER_COUNT / (float)THREAD_COUNT);

pthread_t threads[THREAD_COUNT];
struct range tasks[THREAD_COUNT];

void *behavior(void *argument) {
    struct range *task = (struct range *)argument;
    for (index current = task->start; current <= task->final; current++) {
        task->total += task->array[current];
    }
    pthread_exit(NULL);
}

// Displaying task descriptors
void show(struct range array[], count length) {
    for (index i = 0; i < length; i++) {
        count len = array[i].final - array[i].start + 1;
        printf("@%ld: (%i elts) %i -> %i = %ld\n", i, len, array[i].start, array[i].final, array[i].total);
    }
}

index minimum(index one, index two) {
    return one <= two ? one : two;
}

unsigned long numbers[NUMBER_COUNT];

// Generation of random numbers
void generate() {
    srand(time(0));
    for (index i = 0; i < NUMBER_COUNT; i++) {
        numbers[i] = rand() % 256;
    }
}

int main(int argc, char *argv[]) {
    generate();

    printf("Maximum elements per thread: %d\n", max_per_thread);
    for (unsigned int thread_id = 0; thread_id < THREAD_COUNT; thread_id++) {
        tasks[thread_id].array = numbers;
        tasks[thread_id].start = START(thread_id);
        tasks[thread_id].final = minimum(START(thread_id + 1) - 1, NUMBER_COUNT - 1);
        pthread_create(&threads[thread_id], NULL, behavior, (void *)&tasks[thread_id]);
    }

    for (unsigned int thread_id = 0; thread_id < THREAD_COUNT; thread_id++) {
        pthread_join(threads[thread_id], NULL);
    }

    show(tasks, THREAD_COUNT);

    unsigned long total = 0;
    for (unsigned int thread_id = 0; thread_id < THREAD_COUNT; thread_id++) {
        total += tasks[thread_id].total;
    }

    printf("Total: %f\n", (float)total / (float)NUMBER_COUNT);

    exit(EXIT_SUCCESS);
}
