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

// On a essayé jusqu'à 1.000.000.000 pour NUM_ELEMENTS et cela était fonctionnelle.
#define NUM_ELEMENTS 1000000
#define NUM_THREADS 10

// A la suite de multiples tests réalisés, nous avons constaté que le meilleur choix était deux
// #define NUMBER_COUNT 1000000
// #define THREAD_COUNT 10
// #define MAX_DEPTH 5  // Profondeur maximale pour la création de threads
// Temps d'exécution : 219151 microsecondes

// #define NUMBER_COUNT 1000000
// #define THREAD_COUNT 10
// #define MAX_DEPTH 8  // Profondeur maximale pour la création de threads
// Temps d'exécution : 842267 microsecondes

// #define NUMBER_COUNT 1000000
// #define THREAD_COUNT 10
// #define MAX_DEPTH 2  // Profondeur maximale pour la création de threads
// Temps d'exécution : 51858 microsecondes

// #define NUMBER_COUNT 1000000
// #define THREAD_COUNT 10
// #define MAX_DEPTH 0  // Profondeur maximale pour la création de threads
// Temps d'exécution : 167334 microsecondes


// Cas 1: MAX_DEPTH = 5

//     Temps d'exécution : 219151 microsecondes.
//     Avec une profondeur maximale de 5, il semble qu'il y a un bon équilibre entre le parallélisme et le coût de gestion des threads. La création de threads est suffisante pour tirer parti des coeurs multiples sans surcharger le système par un excès de commutation de contexte ou de synchronisation.

// Cas 2: MAX_DEPTH = 8

//     Temps d'exécution : 842267 microsecondes.
//     Augmenter la profondeur maximale à 8 a considérablement augmenté le temps d'exécution. Cela peut être dû à plusieurs facteurs:
//         Surcharge de gestion des threads: À une profondeur de 8, le nombre de threads qui peuvent être créés augmente exponentiellement. Cette surcharge inclut la création de threads, la commutation de contexte, et surtout la synchronisation et la gestion des verrous lors des fusions.
//         Diminution des performances due à la concurrence excessive : Trop de threads tentant d'accéder à la mémoire et de réaliser des opérations en parallèle peuvent causer une saturation de la bande passante mémoire et un ralentissement global dû à la contention.

// Cas 3: MAX_DEPTH = 2

//     Temps d'exécution : 51858 microsecondes.
//     Ce cas montre le temps d'exécution le plus rapide parmi les trois configurations. Limiter la profondeur de création des threads à 2 signifie que le tri par fusion se décompose rapidement en tâches parallèles sans créer un grand nombre de threads. Cela permet de minimiser la surcharge liée à la gestion des threads tout en exploitant suffisamment le parallélisme pour améliorer les performances.
//     Utilisation optimale des ressources : Avec moins de threads, il y a moins de concurrence pour les ressources du système et moins de temps passé en attente de verrous ou en commutation de contexte.

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

    // On s'assure de libérer toute mémoire allouée en cas d'echec d'allocation
    if (!leftTemp || !rightTemp) {
        perror("Échec de l'allocation de mémoire pour la fusion");
        free(leftTemp);
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
