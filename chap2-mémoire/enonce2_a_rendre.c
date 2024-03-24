/*******************************************************************************
* S-INFO-054 --- Solution pour Travail 02                                      *
* Groupe 05                                                                    *
* Membres:                                                                     *
* - Amory Delcampe                                                             *
* - Nathan Rombaut                                                             *
* - Guy Gillain                                                                *
* - Stéphane Delire                                                            *
*******************************************************************************/

/*
Implémentez la gestion de listes chaînées circulaires en mode FIFO. Les 
fonctions rotateToEven et rotateToOdd appliquent une "rotation" sur une liste 
circulaire en avançant au moins d’une position tous les éléments jusqu’à tomber
respectivement sur un élément pair ou impair.
*/

/*============================================================================*/
/*                                INCLUDES                                    */
/*============================================================================*/
#include <stdio.h>
#include <stdlib.h>

/*============================================================================*/
/*                                STRUCTURES                                  */
/*============================================================================*/

//------------------------------------------------------------------------- Node
struct node {
    // Valeur stockée dans le noeud
    signed int value;
    // Pointeur vers le prochain noeud dans la file
    struct node *next;
};

//--------------------------------------------------------------------- Circular
struct circular {
    // Pointeur vers le dernier élément, permet un accès circulaire
    struct node *tail;
    // Nombre d'éléments dans la file circulaire
    unsigned int size;
};

/*============================================================================*/
/*                                PROTOTYPES                                  */
/*============================================================================*/

//------------------------------------------------------------------- mkcircular
// Fonction pour créer une nouvelle file circulaire vide
struct circular* mkcircular() {
    struct circular *cycle = malloc(sizeof(struct circular));
    cycle->tail = NULL;
    cycle->size = 0;
    return cycle;
}

/*============================================================================*/
/*                                FUNCTIONS                                   */
/*============================================================================*/

//----------------------------------------------------------------------- insert
// Fonction pour insérer un nouvel élément dans la file circulaire
void insert(struct circular *cycle, signed int elt) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->value = elt;
    // Si la file est vide
    if (cycle->tail == NULL) {
        // Le nouveau noeud pointe sur lui-même
        newNode->next = newNode;
    } else { // Si la file n'est pas vide
        // Nouveau noeud pointe vers le premier noeud
        newNode->next = cycle->tail->next;
        // Le dernier noeud actuel pointe vers le nouveau noeud
        cycle->tail->next = newNode;
    }
    // Le nouveau noeud devient le dernier noeud
    cycle->tail = newNode;
    cycle->size++;
}

//---------------------------------------------------------------------- extract
// Fonction pour extraire un élément de la file circulaire
signed int extract(struct circular *cycle) {
    // Si la file est vide
    if (cycle->tail == NULL) {
        // Retourne une erreur
        return -1;
    }
    struct node *head = cycle->tail->next;
    signed int value = head->value;
    // Si la file n'a qu'un seul élément
    if (head == cycle->tail) {
        // La file devient vide
        cycle->tail = NULL;
    } else {
        // Le dernier noeud pointe vers le deuxième noeud
        cycle->tail->next = head->next;
    }
    free(head);
    cycle->size--;
    return value;
}

//----------------------------------------------------------------- rotateToEven
// Fonction pour tourner la file jusqu'au prochain élément pair
struct node* rotateToEven(struct circular *cycle) {
    // Si la file est vide
    if (cycle->tail == NULL) return NULL;
    struct node *current = cycle->tail;
    do {
        current = current->next;
        // Retourne le noeud si sa valeur est paire
        if (current->value % 2 == 0) return current;
    }
    // Continue jusqu'à revenir au point de départ
    while (current != cycle->tail);
    // Aucun élément pair trouvé
    return NULL;
}

//------------------------------------------------------------------ rotateToOdd
// Fonction pour tourner la file jusqu'au prochain élément impair
struct node* rotateToOdd(struct circular *cycle) {
    // Si la file est vide
    if (cycle->tail == NULL) return NULL;
    struct node *current = cycle->tail;
    do {
        current = current->next;
        // Retourne le noeud si sa valeur est impaire
        if (current->value % 2 != 0) return current;
    }
    // Continue jusqu'à revenir au point de départ 
    while (current != cycle->tail);
    // Aucun élément impair trouvé
    return NULL;
}

/*============================================================================*/
/*                                MAIN                                        */
/*============================================================================*/

int main() {
    //--------------------------------------------------------------- mkcircular
    struct circular *cycle = mkcircular();
    //------------------------------------------------------------------- insert
    insert(cycle, 7);
    insert(cycle, 6);
    // Insertion d'un doublon.
    insert(cycle, 6);
    insert(cycle, 5);
    insert(cycle, 4);
    insert(cycle, 3);
    insert(cycle, 2);
    insert(cycle, 1);
    //------------------------------------------------------------------ extract
    printf("Extracted: %d\n", extract(cycle));
    //------------------------------------------------------------- rotateToEven
    struct node *evenNode = rotateToEven(cycle);
    if (evenNode) {
        printf("First even value after rotation: %d\n", evenNode->value);
    }
    //-------------------------------------------------------------- rotateToOdd
    struct node *oddNode = rotateToOdd(cycle);
    if (oddNode) {
        printf("First odd value after rotation: %d\n", oddNode->value);
    }

    return 0;
}

/*============================================================================*/
