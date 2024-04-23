#include <stdlib.h> // Inclure la bibliothèque standard pour la fonction malloc
#include <math.h>   // Inclure pour utiliser la fonction floor

typedef unsigned int index;
typedef unsigned int length;

// Prototypes des fonctions
void merge(int array[], index start, index middle, index final);
void merge_sort(int array[], index start, index final);

void merge_sort(int array[], index start, index final) {
    if (start < final) {
        index middle = (start + final) / 2; // Utilisation de la division entière
        merge_sort(array, start, middle);
        merge_sort(array, middle + 1, final);
        merge(array, start, middle, final);
    }
}

void merge(int array[], index start, index middle, index final) {
    length countL = middle - start + 1;
    int *arrayL = (int *)malloc(countL * sizeof(int));
    length countR = final - middle;
    int *arrayR = (int *)malloc(countR * sizeof(int)); // Correction de la taille allouée pour arrayR

    index i, j, k;

    for (i = 0; i < countL; i++) {
        arrayL[i] = array[start + i];
    }
    for (j = 0; j < countR; j++) {
        arrayR[j] = array[middle + 1 + j];
    }

    i = 0; // Index pour le sous-tableau de gauche
    j = 0; // Index pour le sous-tableau de droite
    k = start; // Index pour le tableau principal

    while (i < countL && j < countR) {
        if (arrayL[i] <= arrayR[j]) {
            array[k++] = arrayL[i++];
        } else {
            array[k++] = arrayR[j++];
        }
    }

    // Copie des éléments restants de arrayL, s'il en reste
    while (i < countL) {
        array[k++] = arrayL[i++];
    }

    // Copie des éléments restants de arrayR, s'il en reste
    while (j < countR) {
        array[k++] = arrayR[j++];
    }

    free(arrayL);
    free(arrayR);
}
