#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int buffer_index = 0;

void sigalrm_handler(int signum) {
    if (buffer_index == 0) {
        // If the buffer is empty, end the program
        printf("Exiting program...\n");
        exit(0);
    }


    // Display the accumulated characters
    printf("Accumulated characters: ");
    for (int i = 0; i < buffer_index; i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = buffer[i] - 32; // Replace lowercase letters with uppercase
        }
        printf("%c", buffer[i]);
    }
    printf("\n");

    // Empty the buffer
    buffer_index = 0;

    // Reset the alarm
    alarm(5);
}

int main() {
    // Set up the SIGALRM signal handler
    struct sigaction sa;
    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    // Set the alarm to go off every 5 seconds
    alarm(5);

    // Accumulate keystrokes in the buffer
    while (1) {
        char c = getchar();
        if (c == EOF) {
            break;
        }
        buffer[buffer_index] = c;
        buffer_index++;
        if (buffer_index == BUFFER_SIZE) {
            // If the buffer is full, empty it
            buffer_index = 0;
            printf("Buffer full! Emptying buffer...\n");
        }
    }

    // Display any remaining accumulated characters
    printf("Accumulated characters: ");
    for (int i = 0; i < buffer_index; i++) {
        if (buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = buffer[i] - 32; // Replace lowercase letters with uppercase
        }
        printf("%c", buffer[i]);
    }
    printf("\n");

    return 0;
}
