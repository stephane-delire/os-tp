/*******************************************************************************
* S-INFO-054 --- Solution pour Travail 03                                    *
* Groupe 05                                                                    *
* Membres:                                                                     *
* - Amory Delcampe                                                             *
* - Nathan Rombaut                                                             *
* - Guy Gillain                                                                *
* - Stéphane Delire                                                            *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define NUM_ELEMENTS 250000000
#define NUM_THREADS 10
#define MAX_THREAD_DEPTH 2
#define MAX_RANDOM_VALUE 10000000

typedef unsigned int index_t;
typedef unsigned int count_t;

struct subrange {
    int *array;
    index_t start, end;
    int depth; 
};

pthread_t thread_ids[NUM_THREADS];
struct subrange tasks[NUM_THREADS];
pthread_mutex_t merge_mutex;

void mergeSubarrays(int *array, index_t start, index_t middle, index_t end) {
    index_t leftSize = middle - start + 1;
    index_t rightSize = end - middle;
    int *leftTemp = malloc(leftSize * sizeof(int));
    int *rightTemp = malloc(rightSize * sizeof(int));

    if (!leftTemp || !rightTemp) {
        perror("Échec de l'allocation de mémoire pour la fusion");
        free(leftTemp); // Assurez-vous de libérer toute mémoire allouée si l'autre échoue.
        free(rightTemp);
        exit(EXIT_FAILURE);
    }

    for (index_t i = 0; i < leftSize; i++) {
        leftTemp[i] = array[start + i];
    }
    for (index_t j = 0; j < rightSize; j++) {
        rightTemp[j] = array[middle + 1 + j];
    }

    index_t leftIndex = 0, rightIndex = 0, mergedIndex = start;
    while (leftIndex < leftSize && rightIndex < rightSize) {
        if (leftTemp[leftIndex] <= rightTemp[rightIndex]) {
            array[mergedIndex++] = leftTemp[leftIndex++];
        } else {
            array[mergedIndex++] = rightTemp[rightIndex++];
        }
    }

    // Copie des éléments restants des sous-tableaux si nécessaire
    while (leftIndex < leftSize) {
        array[mergedIndex++] = leftTemp[leftIndex++];
    }
    while (rightIndex < rightSize) {
        array[mergedIndex++] = rightTemp[rightIndex++];
    }

    free(leftTemp);
    free(rightTemp);
}

void recursiveMergeSort(int *array, index_t start, index_t end) {
    if (start < end) {
        index_t middle = start + (end - start) / 2;
        recursiveMergeSort(array, start, middle);
        recursiveMergeSort(array, middle + 1, end);
        mergeSubarrays(array, start, middle, end);
    }
}

void *parallelMergeSort(void *arg) {
    struct subrange *range = (struct subrange *)arg;
    if (range->start < range->end) {
        if (range->depth < MAX_THREAD_DEPTH) {
            index_t mid = range->start + (range->end - range->start) / 2;

            struct subrange left = {range->array, range->start, mid, range->depth + 1};
            struct subrange right = {range->array, mid + 1, range->end, range->depth + 1};

            pthread_t left_thread, right_thread;
            pthread_create(&left_thread, NULL, parallelMergeSort, &left);
            pthread_create(&right_thread, NULL, parallelMergeSort, &right);

            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);

            pthread_mutex_lock(&merge_mutex);  // Verrouillage pour la fusion
            mergeSubarrays(range->array, range->start, mid, range->end);
            pthread_mutex_unlock(&merge_mutex);  // Déverrouillage après la fusion
        } else {
            recursiveMergeSort(range->array, range->start, range->end);
        }
    }
    return NULL;
}

int main() {
    int *randomNumbers = malloc(NUM_ELEMENTS * sizeof(int));

    FILE *fp = fopen("/dev/urandom", "r");
    if (fp == NULL) {
        perror("Échec de l'ouverture de /dev/urandom");
        // Utilisation de srand et rand comme méthode de secours
        printf("Utilisation de srand et rand pour la génération de nombres aléatoires.\n");
        srand(time(NULL));
        for (index_t i = 0; i < NUM_ELEMENTS; i++) {
            randomNumbers[i] = rand() % MAX_RANDOM_VALUE;
        }
    } else {
        unsigned int num;
        for (index_t i = 0; i < NUM_ELEMENTS; i++) {
            if (fread(&num, sizeof(num), 1, fp) != 1) {
                perror("Échec de la lecture de /dev/urandom");
                fclose(fp);
                free(randomNumbers);
                exit(1);
            }
            randomNumbers[i] = num % MAX_RANDOM_VALUE;
        }
        fclose(fp);
    }

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    pthread_mutex_init(&merge_mutex, NULL);
    struct subrange mainRange = {randomNumbers, 0, NUM_ELEMENTS - 1, 0};
    parallelMergeSort(&mainRange);
    pthread_mutex_destroy(&merge_mutex);

    gettimeofday(&endTime, NULL);

    long seconds = (endTime.tv_sec - startTime.tv_sec);
    long microseconds = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);
    double executionTimeSeconds = seconds + microseconds / 1e6;

    for (index_t i = 0; i < NUM_ELEMENTS; i++) {
        printf("%d ", randomNumbers[i]);
    }
    
    free(randomNumbers);
    printf("\n");
    printf("Temps d'exécution : %ld microsecondes (%.6f secondes)\n", microseconds, executionTimeSeconds);
    
    return 0;
}
