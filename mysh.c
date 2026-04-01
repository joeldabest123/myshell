#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

int main (int argc, char* argv[]) {
    if(argc > 2 ) {
        printf("Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    if (argc == 1) {
        //determines if read from std input or argument
    }
}