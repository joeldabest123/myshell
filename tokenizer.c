#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "tokenizer.h"
#include "linkedList.h"




//builds buffer to create file path to be open()'d
char* bufferBuilder(const char *path, struct dirent* entry) {
    int bufferCount = 0;
    char* fileName = entry->d_name;

    bufferCount = strlen(path) + 1 + strlen(fileName) + 1;

    char * tempBuffer = malloc(bufferCount);
    snprintf(tempBuffer, bufferCount, "%s/%s", path, fileName); //filepath created
    
    return tempBuffer;
}

//goes through each word in buffer, filters, and sticks said word into linked lists inside the master array
void buffToList(char* buffer, off_t fileSize, List* words) {

    int start = 0;
    int end = 0;
    int wordCounter = 0;
    char* word;

    while(buffer[start] == ' ') { //clears out inital spaces in file
        start++;
    }

    for(end = 0; end < fileSize; end++) { //goes through every word in file and filters
        if((buffer[end]) == ' ' || buffer[end] == '\0') {
            buffer[end] = '\0';
            if (end > start) {
                word = malloc(end + 1 - start);
                int counter = 0;
                for(int i = start; i < end; i++) {
                    if(isalnum(buffer[i]) != 0 || buffer[i] == '-') { //filters out anything not a letter, num, or hyphen
                        word[counter] = tolower(buffer[i]);
                        counter++;
                    }
                }
                word[counter] = '\0'; //replaces spaces with null terminators

                if (counter > 0) {
                    insert(words, word); //inserts given the given word into a node
                    wordCounter++;
                }
                free(word);
            }
                start = end + 1;

            while(start < fileSize && buffer[start] == ' ') { //moves start to next letter and skips spaces
                start++;
            }
            end = start - 1;
            
        }
        
    }
    if(end > start) {
        word = malloc(end + 1 - start);
        int counter = 0;
        for(int i = start; i < end; i++) {
            if(isalnum((unsigned char)buffer[i]) || buffer[i] == '-') {
                word[counter] = tolower(buffer[i]);
                counter++;
            }
        }
        word[counter] = '\0';
        if(counter > 0) {
            insert(words, word);
        }
        free(word);
    }
}

//opens a given file, goes through it, and tokenizes words into nodes
void tokenize(const char *file, off_t fileSize, List* words) {
    int fd = open(file, O_RDONLY); //opens file to read from
    char* buffer = malloc(fileSize + 1); //creates a buffer to store all elements read from file

    if(fd == -1) {
        perror("Error opening file");
        return;
    }

    int check = read(fd, buffer, fileSize);
    buffer[fileSize] = '\0';

    if(check == -1) {
        perror("tokenizing error");
        return;
    }

    buffToList(buffer, fileSize, words); //uses as child process to tokenize and filter buffer

    free(buffer); //frees the buffer
    close(fd); //closes file
}

//goes through files and directories to determine which is which, then tokenizes said files and whatnot
void paceDirectories(const char *path, List*** allFiles, int* capacity, int *count) {
    DIR* d = opendir(path);

    if(d == NULL) {
        perror("Error opening Directory");
        return;
    }

    struct dirent* entry;
    char* fileName;

    while((entry = readdir(d)) != NULL) { //goes until the directory is empty


        fileName = entry->d_name;
        if(strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0 || fileName[0] == '.') { //filters out hidden files and directories
            continue;
        }

        char* fullPath = bufferBuilder(path, entry); //builds path to open file
        struct stat st;

        stat(fullPath, &st); //alows the ability to look at file metadata
        
        if(S_ISREG(st.st_mode)) { //determines if a file or directory
            int entryLength = strlen(fileName);
            if(entryLength >= 4) {
                char* suffix = fileName + (entryLength - 4);

                if(strcmp(suffix, ".txt") == 0) { //determines if a text file
                    List* words = initializeList(fullPath); //initializes a List for all words in file
                    tokenize(fullPath, st.st_size, words); //tokenizes words from file
                    (*allFiles)[*count] = words; //sticks list into master array
                    (*count)++;
                    if(*count == *capacity) {
                        *allFiles = lengthenArray(*allFiles, capacity); //lengthens array if too short
                    }
                };
            }
        } else {
            if (S_ISDIR(st.st_mode)) { //if file is a directory, enters file through recursion and paces through it
                paceDirectories(fullPath, allFiles, capacity, count);
            }
        }

        free(fullPath); //frees the buffer stored in fulPath

    }

    closedir(d); //closes directory and all recursive directories
}
