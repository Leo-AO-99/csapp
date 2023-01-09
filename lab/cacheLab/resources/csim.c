#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>

char filePath[1010];
int s, E, b;
int S; // total amount of set
FILE *file;
int hits, misses, evictions;
int debug = 0;

typedef struct Node
{
    unsigned int *tag;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct LRU
{
    int *size;
    Node *head;
    Node *tail;
} LRU;

typedef struct Cache
{
    LRU *Sets;
} Cache;

Cache cache;

void init()
{
    cache.Sets = malloc(S * sizeof(LRU));
    for (int i = 0; i < S; i++)
    {
        cache.Sets[i].head = malloc(sizeof(Node));
        cache.Sets[i].tail = malloc(sizeof(Node));
        cache.Sets[i].size = malloc(sizeof(unsigned int));
        *(cache.Sets[i].size) = 0;

        cache.Sets[i].head->next = cache.Sets[i].tail;
        cache.Sets[i].tail->prev = cache.Sets[i].head;
    }
}

void update(unsigned int address)
{
    unsigned int targetSet = ((1 << s) - 1) & (address >> b);
    unsigned int targetTag = address >> (s + b);

    if(debug)
        printf("--targetSet: %x, targetTag: %x\n", targetSet, targetTag);

    LRU lru = cache.Sets[targetSet];

    Node *cur = lru.head->next;
    int flag = 0;
    while (cur != lru.tail)
    {
        if (*(cur->tag) == targetTag)
        {
            flag = 1;
            break;
        }
        cur = cur->next;
    }

    if (flag)
    {
        // hit the cache
        hits++;

        // move cur to the head of LRU
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        cur->prev = lru.head;
        cur->next = lru.head->next;
        cur->next->prev = cur;
        lru.head->next = cur;
        if (debug)
            printf("--hit\n");
    }
    else
    {
        // miss the cache
        misses++;
        if (debug)
            printf("--miss\n");

        if (*(lru.size) == E) // evict
        {
            if (debug)
                printf("--eviction\n");
            evictions++;

            cur = cur->prev;

            *(cur->tag) = targetTag;
            cur->next->prev = cur->prev;
            cur->prev->next = cur->next;

            cur->prev = lru.head;
            cur->next = lru.head->next;
            cur->next->prev = cur;
            lru.head->next = cur;
        }
        else
        {
            Node *newNode = malloc(sizeof(Node));
            newNode->tag = malloc(sizeof(unsigned int));
            *(newNode->tag) = targetTag;
            newNode->prev = lru.head;
            newNode->next = lru.head->next;
            newNode->next->prev = newNode;
            lru.head->next = newNode;
            *(lru.size) = *(lru.size) + 1;
        }
    }
}

int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "vs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'v':
            debug = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            memcpy(filePath, optarg, sizeof filePath);
            break;
        }
    }

    file = fopen(filePath, "r");
    if (file == NULL)
    {
        printf("illegal filePath\n");
        exit(0);
    }

    S = 1 << s;

    init();

    char op;
    unsigned int address;
    int size;
    while ((fscanf(file, " %c %x,%d", &op, &address, &size)) > 0)
    {
        if (debug)
            printf("%c %x,%d\n", op, address, size);

        switch (op)
        {
        case 'L':
            update(address);
            break;
        case 'M':
            update(address);
        case 'S':
            update(address);
            break;
        }
    }

    fclose(file);

    printSummary(hits, misses, evictions);
    return 0;
}