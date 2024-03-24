/*============================================================================*/
// La réponse de l'exo 6 est pratiquement la réponse de l'exo demandé !
// sauf qu'a la place d'une simple liste FIFO, on a une liste circulaire,
// pour laquelle il faut ajouter des fonctions pour tourner la liste vers les 
// éléments pairs et impairs.


/*
Il faut garder cette structure pour l'implémentation de la file circulaire : 
struct circular * mkcircular ();
void insert ( struct circular * cycle , signed int elt );
signed int extract ( struct circular * cycle );
struct node * rotateToEven ( struct circular * cycle );
struct node * rotateToOdd ( struct circular * cycle );
*/


/*============================================================================*/
#include <stdio.h> // Inclut les définitions standard d'entrée/sortie, comme printf()
#include <stdlib.h> // Inclut les fonctions de gestion de la mémoire dynamique, comme malloc() et free()

// Définition de la structure pour un élément de file (queue)
struct fifo {
    unsigned int size; // Stocke le nombre d'éléments dans la file
    struct node *head; // Pointeur vers le premier élément de la file
};

// Définition de la structure pour un noeud de la file
struct node {
    signed int value; // Valeur stockée dans le noeud
    struct node *next; // Pointeur vers le prochain noeud dans la file
};

// Fonction pour créer une nouvelle file vide
struct fifo* mkfifo() {
    struct fifo *lst = malloc(sizeof(struct fifo)); // Alloue de la mémoire pour la file
    lst->size = 0; // Initialise le compteur de taille à 0
    lst->head = NULL; // Initialise la tête de la file à NULL (file vide)
    return lst; // Retourne le pointeur vers la nouvelle file
}

// Fonction pour créer un nouveau noeud avec une valeur spécifiée
struct node* mknode(signed int v) {
    struct node *nd = malloc(sizeof(struct node)); // Alloue de la mémoire pour le noeud
    nd->value = v; // Définit la valeur du noeud
    nd->next = NULL; // Le nouveau noeud n'a pas de successeur
    return nd; // Retourne le pointeur vers le nouveau noeud
}

// Fonction pour ajouter (enfiler) un élément à la fin de la file
void enqueue(struct fifo *lst, signed int elt) {
    struct node *newNode = mknode(elt); // Crée un nouveau noeud avec l'élément
    if (lst->head == NULL) { // Si la file est vide
        lst->head = newNode; // Le nouveau noeud devient le premier élément
    } else { // Si la file n'est pas vide
        struct node *predecessor = lst->head; // Trouve le dernier élément de la file
        while (predecessor->next != NULL) {
            predecessor = predecessor->next;
        }
        predecessor->next = newNode; // Lie le dernier noeud au nouveau noeud
    }
    lst->size++; // Incrémente le compteur de taille de la file
}

// Fonction pour retirer (défiler) l'élément en tête de la file
signed int dequeue(struct fifo *lst) {
    if (lst->head == NULL) { // Si la file est vide
        return -1; // Retourne une erreur
    }
    struct node *current = lst->head; // Sauvegarde le premier noeud
    lst->head = current->next; // Avance la tête de la file
    lst->size--; // Décrémente le compteur de taille
    signed int v = current->value; // Sauvegarde la valeur à retourner
    free(current); // Libère la mémoire du noeud retiré
    return v; // Retourne la valeur du noeud retiré
}

// Fonction pour afficher le contenu de la file
void print(char *label, struct fifo *lst) {
    printf("%s (size=%u):\n", label, lst->size); // Affiche l'étiquette et la taille de la file
    if (lst->head == NULL) { // Si la file est vide
        printf("[]\n"); // Affiche une file vide
    } else { // Si la file contient des éléments
        struct node *current = lst->head; // Parcourt les éléments de la file
        printf("[");
        while (current != NULL) {
            printf("%d ", current->value); // Affiche la valeur de chaque élément
            current = current->next; // Passe au noeud suivant
        }
        printf("]\n"); // Ferme la représentation textuelle de la file
    }
}

// Fonction principale du programme
int main(int argc, char *argv[]) {
    struct fifo *fifo1 = mkfifo(); // Crée une nouvelle file
    // Ajoute des éléments à la file
    enqueue(fifo1, 42);
    enqueue(fifo1, 27);
    enqueue(fifo1, 81);
    // Affiche la file après l'ajout des
    //éléments et les retire un par un, en affichant la file après chaque opération.
    print("fifo1", fifo1); // Affiche le contenu de la file après l'ajout des éléments
    printf("Got: %i\n", dequeue(fifo1)); // Retire et affiche la valeur du premier élément ajouté (42)
    print("fifo1", fifo1); // Affiche la file après avoir retiré un élément

    printf("Got: %i\n", dequeue(fifo1)); // Retire et affiche la valeur du deuxième élément ajouté (27)
    print("fifo1", fifo1); // Affiche la file après avoir retiré un deuxième élément

    printf("Got: %i\n", dequeue(fifo1)); // Retire et affiche la valeur du troisième élément ajouté (81)
    print("fifo1", fifo1); // Affiche la file après avoir retiré un troisième élément

    free(fifo1); // Libère la mémoire allouée pour la file
    return EXIT_SUCCESS; // Termine le programme avec un statut de succès
}
