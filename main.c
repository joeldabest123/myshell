#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 1024

int main (int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];

    int isInteractive = isatty(STDIN_FILENO);
    if (interactive) {
        printf("Welcome!\n");
    }

    while(1) {
        write(STDOUT_FILENO, "$ ", 2); //for the prompt

        ssize_t bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);

        if(bytes <= 0) break;

        buffer[bytes] = '\0';

        //Gotta process commands
    }
    
    if (interactive) {
        printf("Exiting...\n");
    }

    return 0;
}