#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

// Taille buffer de 1024 octets pour éviter dépassement (1024 caractères)
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int bufferIndex = 0;
struct termios params_termios;

void disableRawMode() {
    /*
    On applique les anciens paramètres du terminal contenus dans param_termios à l'entrée standard STDIN_FILENO
    TCSAFLUSH assure que les modifications prennent effet immédiatement.
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &params_termios);
}

void enableRawMode() {
    /*
    Récupère les paramètres actuels du terminal. Cette fonctionnalité est définie dans la bibliothèque termios.h
    STDIN_FILENO représente l'entrée standard.
    &params_termios récupère les paramètres stocké dans la structure termios
    */
    tcgetattr(STDIN_FILENO, &params_termios);

    /*
    Implémenté dans stdlib.h
    Utilisé pour enregistrer une fonction à être appelée automatiquement à la fin de l'exécution d'un programme.
    Passe un pointeur vers la fonction qu'on souhaite appeler à la fin du programme. (elle doit contenir aucun argument et rien retourner)
    */
    atexit(disableRawMode);
    
    /*
    On initialise les valeurs récupérées précédemment dans la structure termios.
    On modifie les paramètres locaux c_flag en désactivant les modes suivants :
    - ECHO: Permet de désactiver l'affichage des caractères automatiquement à l'écran
    - ICANON : Permet de lire les caractères immédiatement après qu'ils sont tapés sans attendre une nouvelle ligne (finalement, évite l'attente de l'apui sur la touche 'Enter')
    L'opérateur &= permet de désactiver à la fois ECHO et ICANON tout en laissant les autres bits intactes dans c_lflag
    */
    struct termios raw = params_termios;
    raw.c_lflag &= ~(ECHO | ICANON);

    /*
    On applique les nouveaux paramètres du terminal contenus dans raw à l'entrée standard STDIN_FILENO
    TCSAFLUSH assure que les modifications prennent effet immédiatement.
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void handle_alarm(int sig) {
    if (bufferIndex == 0) {
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

int main() {
    enableRawMode(); // Active le mode non canonique pour permettre la lecture immédiate des caractères.
    signal(SIGALRM, handle_alarm);
    alarm(5); // Configure le premier SIGALRM pour se déclencher dans 5 secondes.

    while (1) {
        if (bufferIndex < BUFFER_SIZE - 1) {
            int c = getchar(); // Utilise getchar() pour lire un caractère.
            if (c != EOF) {
                buffer[bufferIndex++] = c;
            }
        }
    }

    return 0; // Ce point ne sera jamais atteint.
}