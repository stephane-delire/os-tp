#include <stdio.h >  // p r i n t f ( )
#include <stdlib.h > // EXIT_SUCCESS, malloc ( )
struct circular
{
    unsigned int size;
    struct node *head;
};
struct node
{
    signed int value;
    struct node *next;
};
struct circular *mkcircular()
{
    struct circular *lst = malloc(sizeof(struct circular));
    lst -> size = 0;
    lst -> head = NULL;
    return lst;
}
struct node *mknode(signed int v)
{
    struct node *nd = malloc(sizeof(struct node));
    nd -> value = v;
    nd -> next = NULL;
    return nd;
}

void insert(struct circular *cycle, signed int elt)
{
    struct node *new = mknode(elt);
    if (cycle->head == NULL){
        cycle->head = new;
    }
    else
    {
        struct node *predecessor = cycle->head;
        while (predecessor->next != cycle->head)
            predecessor = predecessor -> next;
        predecessor -> next = new;
    }
    new->next = cycle->head;
    cycle->size++;
}

signed int extract(struct circular *cycle)
{
    struct node *current = cycle->head;
    cycle->head = current->next;
    cycle->size--;
    signed int v = current -> value;
    free(current);

    struct node *predecessor = cycle->head;
    while (predecessor->next != cycle->head)
        predecessor = predecessor->next;

    predecessor->next = cycle->head;
    return v;
}

struct node *rotateToEven(struct circular *cycle)
{
    struct node *predecessor = cycle->head;
    while (predecessor->next != cycle->head){
        if(predecessor->value %2 ==0){
            return predecessor;
        }
        predecessor = predecessor->next;
    }
}
    

struct node *rotateToOdd(struct circular *cycle)
{
    struct node *predecessor = cycle->head;
    while (predecessor->next != cycle->head)
    {
        if (predecessor->value % 2 == 1)
        {
            return predecessor;
        }
        predecessor = predecessor->next;
    }
}

int main(int argc, char *argv[])
{
    struct circular *fifo1 = mkfifo();
    insert(fifo1, 42);
    insert(fifo1, 27);
    insert(fifo1, 81);
    print(" fifo1 ", fifo1);
    printf(" Got : % i \ n", extract(fifo1));
    print(" fifo1 ", fifo1);
    printf(" Got : % i \ n", extract(fifo1));
    print(" fifo1 ", fifo1);
    printf(" Got : % i \ n", extract(fifo1));
    print(" fifo1 ", fifo1);
    free(fifo1);
    return EXIT_SUCCESS;
}
