/******************************************************************************
* S-INFO-054 --- Solution pour Travail 01                                     *
* Groupe 05                                                                   *
* Membres:                                                                    *
* - Amory Delcampe                                                            *
* - Nathan Rombaut                                                            *
* - Guy Gillain                                                               *
* - Stéphane Delire                                                           *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

// Taille buffer de 1024 octets pour éviter le dépassement
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int bufferIndex = 0;
struct termios params_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &params_termios) != 0) {
        perror("tcsetattr"); // Affiche une erreur si tcsetattr échoue
        exit(EXIT_FAILURE); // Termine immédiatemment le programme en indiquant qu'une tâche a echouée > on améliore la portabilité entre les différents systèmes en utilisant EXIT_FAILURE
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &params_termios) != 0) {
        perror("tcgetattr"); // Affiche une erreur si tcgetattr échoue
        exit(EXIT_FAILURE);
    }
    atexit(disableRawMode);
    struct termios raw = params_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0) {
        perror("tcsetattr"); // Affiche une erreur si tcsetattr échoue
        exit(EXIT_FAILURE);
    }
}

void handleAlarm(int sig) {
    if (bufferIndex == 0) { // Le buffer est vide donc, on termine le programme.
        printf("Buffer vide, programme terminé.\n");
        exit(0);
    } else {
        for (int i = 0; i < bufferIndex; ++i) {
            putchar(toupper(buffer[i]));
        }
        printf("\n");
        bufferIndex = 0; // Réinitialise le buffer pour la prochaine accumulation.
    }
    alarm(5); // Reprogramme l'alarme pour dans 5 secondes.
}

void setupAlarmHandler() {
    struct sigaction sa;
    sa.sa_handler = handleAlarm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Pas de flags spécifiques

    if (sigaction(SIGALRM, &sa, NULL) != 0) {
        perror("sigaction"); // Affiche une erreur si sigaction échoue
        exit(EXIT_FAILURE);
    }
}

int main() {
    enableRawMode(); // Active le mode non canonique pour permettre la lecture immédiate des caractères.
    setupAlarmHandler();
    alarm(5); // Configure le premier SIGALRM pour se déclencher dans 5 secondes.

    while (1) {
        if (bufferIndex < BUFFER_SIZE - 1) { // Vérifie que l'index ne dépasse pas la taille du buffer
            int c = getchar(); // Utilise getchar() pour lire un caractère.
            if (c != EOF) { // Vérifie que le caractère lu n'est pas la fin de fichier (EOF) avant de l'ajouter au buffer.
                buffer[bufferIndex++] = c;
            }
        }
    }

    return 0;
}
