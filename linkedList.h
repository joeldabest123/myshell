#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node {
    char* data;
    double counter;
    double mean;
    struct Node *next;
    short seen;
} Node;

typedef struct linkedList {
    int totalCount;
    Node *head;
    char* name;

} List;

List** initializeArray ();
List** lengthenArray (List **, int*);
List* initializeList (const char*);
Node* initializeNode (List*, const char*);
Node* search(List *, const char *);
void alphabetical(List *, const char *);
void insert (List *, const char *);
void clearList (List**, int);

#endif