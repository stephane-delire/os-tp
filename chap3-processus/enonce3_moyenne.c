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
#include <limits.h>
#include <unistd.h>
#include <time.h>

#define NUM_ELEMENTS 10000000
#define NUM_THREADS 10
#define MAX_THREAD_DEPTH 6
#define MAX_RANDOM_VALUE 100000000

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
        perror("Échec d'allocation de mémoire pour la fusion");
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

            mergeSubarrays(range->array, range->start, mid, range->end);
        } else {
            recursiveMergeSort(range->array, range->start, range->end);
        }
    }
    return NULL;
}

int main() {
    int meilleur_nb_threads = 0;
    int meilleure_profondeur = 0;
    long meilleur_temps = LONG_MAX;

    for (int nb_threads = 1; nb_threads <= NUM_THREADS; nb_threads++) {
        for (int profondeur_max = 1; profondeur_max <= MAX_THREAD_DEPTH; profondeur_max++) {
            long total_microsecondes = 0;
            int nb_essais = 5;

            for (int essai = 0; essai < nb_essais; essai++) {
                int *nombresAleatoires = malloc(NUM_ELEMENTS * sizeof(int));
                if (!nombresAleatoires) {
                    perror("Échec de l'allocation de mémoire pour les nombres aléatoires");
                    return EXIT_FAILURE;
                }

                FILE *fp = fopen("/dev/urandom", "r");
                if (fp == NULL) {
                    perror("Échec de l'ouverture de /dev/urandom");
                    srand(time(NULL));
                    for (index_t i = 0; i < NUM_ELEMENTS; i++) {
                        nombresAleatoires[i] = rand() % MAX_RANDOM_VALUE;
                    }
                } else {
                    unsigned int num;
                    for (index_t i = 0; i < NUM_ELEMENTS; i++) {
                        if (fread(&num, sizeof(num), 1, fp) != 1) {
                            perror("Échec de la lecture de /dev/urandom");
                            fclose(fp);
                            free(nombresAleatoires);
                            return EXIT_FAILURE;
                        }
                        nombresAleatoires[i] = num % MAX_RANDOM_VALUE;
                    }
                    fclose(fp);
                }

                struct timeval debut, fin;
                gettimeofday(&debut, NULL);

                struct subrange plagePrincipale = {nombresAleatoires, 0, NUM_ELEMENTS - 1, 0};
                parallelMergeSort(&plagePrincipale);

                gettimeofday(&fin, NULL);

                long secondes = (fin.tv_sec - debut.tv_sec);
                long microsecondes = ((secondes * 1000000) + fin.tv_usec) - (debut.tv_usec);
                total_microsecondes += microsecondes;

                free(nombresAleatoires);
            }

            long temps_moyen = total_microsecondes / nb_essais;
            printf("Nb Threads: %d, Profondeur Max: %d, Temps Moyen: %ld microsecondes\n", nb_threads, profondeur_max, temps_moyen);

            if (temps_moyen < meilleur_temps) {
                meilleur_temps = temps_moyen;
                meilleur_nb_threads = nb_threads;
                meilleure_profondeur = profondeur_max;
            }
        }
    }

    printf("Meilleure configuration: Nb Threads = %d, Profondeur Max = %d avec un temps moyen de %ld microsecondes\n", meilleur_nb_threads, meilleure_profondeur, meilleur_temps);
    
    return 0;
}
