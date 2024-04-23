#include <stdio.h>
#include <stdlib.h>

typedef unsigned int index;
typedef unsigned int length;

void merge(int array[], index start, index middle, index final);
void merge_sort(int array[], index start, index final);

void merge_sort(int array[], index start, index final) {
    if (start < final) {
        index middle = (start + final) / 2;
        merge_sort(array, start, middle);
        merge_sort(array, middle + 1, final);
        merge(array, start, middle, final);
    }
}

void merge(int array[], index start, index middle, index final) {
    length countL = middle - start + 1;
    int *arrayL = malloc(countL * sizeof(int));
    index currentL, currentR;

    for (currentL = 0; currentL < countL; currentL++) {
        arrayL[currentL] = array[start + currentL];
    }

    length countR = final - middle;
    int *arrayR = malloc(countR * sizeof(int));

    for (currentR = 0; currentR < countR; currentR++) {
        arrayR[currentR] = array[middle + 1 + currentR];
    }

    currentL = 0;
    currentR = 0;
    index current;

    for (current = start; current <= final && currentL < countL && currentR < countR; current++) {
        if (arrayL[currentL] <= arrayR[currentR]) {
            array[current] = arrayL[currentL++];
        } else {
            array[current] = arrayR[currentR++];
        }
    }

    while (currentR < countR) {
        array[current++] = arrayR[currentR++];
    }

    while (currentL < countL) {
        array[current++] = arrayL[currentL++];
    }

    free(arrayL);
    free(arrayR);
}

int main() {
    int array[] = {9, 3, 4, 1, 7, 5, 8, 2, 6, 0};
    int n = sizeof(array) / sizeof(array[0]);

    printf("Array avant le tri:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    merge_sort(array, 0, n - 1);

    printf("Array après le tri:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
