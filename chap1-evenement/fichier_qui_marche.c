#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>

#define BUFFER_SIZE 100

char buffer[BUFFER_SIZE];
int buffer_index = 0;

// Fonction de gestionnaire de signal pour SIGALRM
void alarm_handler(int signo) {
    if (buffer_index > 0) {
        // Convertir les minuscules en majuscules
        for (int i = 0; i < buffer_index; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        // Afficher le contenu du buffer
        printf("Contenu du buffer: %s\n", buffer);

        // Vider le buffer
        buffer_index = 0;
    } else {
        // Si le buffer est vide, terminer le programme
        printf("Le buffer est vide. Fin du programme.\n");
        exit(0);
    }

    // Réinitialiser le signal d'alarme
    alarm(5);
}

int main() {
    // Installer le gestionnaire de signal pour SIGALRM
    signal(SIGALRM, alarm_handler);

    // Activer le signal d'alarme pour la première fois
    alarm(5);

    while (1) {
        // Lire les frappes au clavier à l'aide de getchar()
        char c = getchar();

        // Vérifier si le caractère est une lettre et si le buffer n'est pas plein
        if (isalpha(c) && buffer_index < BUFFER_SIZE - 1) {
            buffer[buffer_index++] = c;
        }
    }

    return 0;
}
