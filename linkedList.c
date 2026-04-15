#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "linkedList.h"

//initializers a linkedList with full setup
List* initializeList (const char* path) {

    List* l = malloc(sizeof(List));

    if (l == NULL) {
        perror("malloc failed"); //precautionary for if malloc fails
        return NULL;
    }
    l->totalCount = 0;
    l->head = NULL;
    l->name = strdup(path);
    return l;
}

//initialized the array of LinkedLists (The master array)
List** initializeArray () {
    List **allFiles = malloc(10 * sizeof(List*));

    if(allFiles == NULL) {
        perror("malloc failed");
        return NULL;
    }

    return allFiles;
}

    // doubling the size of the array for more storage
List** lengthenArray (List **allFiles, int* capacity) {
    *capacity = (*capacity) * 2;
    
    //reallocating by doing the number of slots * 8 bytes (size of a pointer)
    List **temp = realloc(allFiles, (*capacity) * sizeof(List*));

    if(temp == NULL) {
        perror("realloc failed"); //kinda self explanatory
        return allFiles;
    }
    return temp;
}

//sets up nodes
Node* initializeNode (List* l, const char* word) {
    Node* n = malloc(sizeof(Node));

    if(n == NULL) {
        return NULL;
    }
    n->data = strdup(word); //sets up a node for each word
    n->counter = 1; //the amount of times the word is called per file
    n->mean = 0; //the mean of the word compared to all words in the file
    n->seen = 0; //if the word has been paced through or not
    n->next = NULL; //the next node/word in line
    l->totalCount++; //the total count of words in the list is incremented
    return n;
}


//searched for a word in a given linkedList
Node* search(List *l, const char *key) {
    Node *ptr = l->head;
    int i = 0;

    while(i < l->totalCount) {
        if(strcmp(ptr->data, key) == 0) {
            return ptr;
        }
        ptr = ptr->next;
        i++;
    }
    return NULL;
}

//alphabetizes the list so the nodes are in order (I despised writing this)
void alphabetical(List *l, const char *key) {

    Node* ptr = l->head;
    Node* prev = NULL;

    while(ptr != NULL) { //loops through all nodes
        int firstletkey = (int)key[0]; //grabs the first letter of the key search word
        int firstletnode =(int) ptr->data[0]; //grabs the first letter of the given node

        //if key is smaller, spot found
        if(firstletkey < firstletnode) {
            break;
        }

        //if first letters match, keep going
        if(firstletkey == firstletnode) {
            int j = 0;
            int foundSpot = 0;
            while(1) {
                int c1 = (int)key[j];
                int c2 =(int) ptr->data[j];

                if(c1 < c2 || c1 == 0) { //key is shorter/smaller
                    foundSpot = 1;
                    break;
                }
                if(c1 > c2 || c2 == 0) { //key is longer or larger
                    break;
                }
                j++;
            }
            if(foundSpot) {
                break;
            }
        }
        
        prev = ptr;
        ptr = ptr->next;
    }

    Node* smaller = initializeNode(l, key);
    smaller->next = ptr;

    if(prev == NULL) {
        l->head = smaller; //sets new head
    } else {
        prev->next = smaller; //in the middle or end
    }

}

//inserts a node into the list
void insert (List *l, const char *key) {
    if(l->head == NULL) {
        l->head = initializeNode(l, key);
        
        return;
    }

    Node* found = search(l, key);

    if(found != NULL) {
        found->counter = found->counter + 1;
        return;
    }

    alphabetical(l, key); //calls alphabetical to sort the list

    
}

//frees all malloc'd values from master list and individual lists within
void clearList (List** allFiles, int fileCount) {

    int counter = 0;
    while(counter < fileCount) {
        List* temp = allFiles[counter];

        Node* ptr = temp->head;
        Node* next;

        while(ptr != NULL) {
            next = ptr->next;
            free(ptr->data); //frees the word
            free(ptr); //frees the node
            ptr = next;
        }
        free(temp->name); //frees path name
        free(temp);

        counter++;
    }
    free(allFiles);
}
