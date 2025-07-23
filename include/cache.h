#ifndef CACHE_H
#define CACHE_H

#include <time.h>

typedef struct cache_element
{
    char *data;
    int len;
    char *url;
    time_t lru_time_track;
    struct cache_element *next;
} cache_element;

void init_cache();

cache_element *find(char *url);

int add_cache_element(char *data, int size, char *url);

void remove_cache_element();

#endif
