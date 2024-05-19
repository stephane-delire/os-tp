#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_ELEMENTS 1000000 // Définit le nombre total d'éléments à trier
#define MAX_THREADS 10        // Nombre maximal de threads utilisables
#define NUM_TESTS 10          // Nombre de tests effectués pour chaque configuration de thread

typedef unsigned int index_t;

struct subrange {
    int *array;
    index_t start, end;
};

// Fusionne deux sous-segments d'un tableau en un seul segment trié
void merge(int *array, index_t start, index_t middle, index_t end) {
    index_t n1 = middle - start + 1;
    index_t n2 = end - middle;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    if (L == NULL || R == NULL) {
        fprintf(stderr, "Échec de l'allocation mémoire lors de la fusion.\n");
        free(L);
        free(R);
        exit(EXIT_FAILURE);
    }

    for (index_t i = 0; i < n1; i++) L[i] = array[start + i];
    for (index_t j = 0; j < n2; j++) R[j] = array[middle + 1 + j];

    index_t i = 0, j = 0, k = start;
    while (i < n1 && j < n2) {
        array[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }

    while (i < n1) array[k++] = L[i++];
    while (j < n2) array[k++] = R[j++];

    free(L);
    free(R);
}

// Fonction exécutée par chaque thread pour trier son segment du tableau
void *merge_sort(void *arg) {
    struct subrange *range = (struct subrange *)arg;
    if (range->start < range->end) {
        index_t mid = range->start + (range->end - range->start) / 2;
        struct subrange left = {range->array, range->start, mid};
        struct subrange right = {range->array, mid + 1, range->end};

        merge_sort(&left);
        merge_sort(&right);
        merge(range->array, range->start, mid, range->end);
    }
    return NULL;
}

// Fonction principale qui configure et exécute les tests de tri fusion parallèle
int main() {
    struct timeval start_time, end_time;
    double best_time = __DBL_MAX__;
    int best_thread_count = 0;

    // Teste différentes configurations de threads de 1 à MAX_THREADS
    for (int num_threads = 1; num_threads <= MAX_THREADS; num_threads++) {
        double total_time = 0.0;

        printf("Configuration avec %d threads:\n", num_threads);
        // Répète le test NUM_TESTS fois pour obtenir une moyenne fiable
        for (int test = 0; test < NUM_TESTS; test++) {
            int *array = malloc(NUM_ELEMENTS * sizeof(int));
            if (!array) {
                fprintf(stderr, "Échec d'allocation de mémoire pour le tableau.\n");
                return 1;
            }

            // Remplissage du tableau avec des nombres aléatoires
            srand(time(NULL));
            for (index_t i = 0; i < NUM_ELEMENTS; i++) {
                array[i] = rand() % 100000;
            }

            gettimeofday(&start_time, NULL);

            pthread_t threads[num_threads];
            struct subrange ranges[num_threads];
            index_t length = NUM_ELEMENTS / num_threads;

            // Création des threads pour effectuer le tri fusion en parallèle
            for (int i = 0; i < num_threads; i++) {
                ranges[i].array = array;
                ranges[i].start = i * length;
                ranges[i].end = (i + 1) * length - 1;
                if (i == num_threads - 1) ranges[i].end = NUM_ELEMENTS - 1;

                if (pthread_create(&threads[i], NULL, merge_sort, &ranges[i]) != 0) {
                    fprintf(stderr, "Échec de la création du thread %d\n", i);
                    free(array);
                    return 1;
                }
            }

            // Attente de la fin de tous les threads
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);
            }

            gettimeofday(&end_time, NULL);
            long seconds = (end_time.tv_sec - start_time.tv_sec);
            long microseconds = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
            double execution_time = microseconds / 1e6;
            total_time += execution_time;

            printf("Test %d: Temps d'exécution = %.6f secondes\n", test + 1, execution_time);

            free(array);
        }

        double average_time = total_time / NUM_TESTS;
        printf("Temps moyen pour %d threads = %.6f secondes\n\n", num_threads, average_time);

        // Enregistrement du meilleur temps et du nombre de threads correspondant
        if (average_time < best_time) {
            best_time = average_time;
            best_thread_count = num_threads;
        }
    }

    printf("Meilleur configuration: %d threads avec un temps moyen d'exécution de %.6f secondes\n", best_thread_count, best_time);
    return 0;
}
