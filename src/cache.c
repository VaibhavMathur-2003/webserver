#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "cache.h"

#define MAX_SIZE 200*(1<<20)           
#define MAX_ELEMENT_SIZE 10*(1<<20)    


static cache_element* head = NULL;
static int cache_size = 0;
static pthread_mutex_t lock;


void init_cache() {
    pthread_mutex_init(&lock, NULL);
    head = NULL;
    cache_size = 0;
}

cache_element* find(char* url) {
    cache_element* site = NULL;
    pthread_mutex_lock(&lock);

    if (head != NULL) {
        site = head;
        while (site != NULL) {
            if (strcmp(site->url, url) == 0) {
                site->lru_time_track = time(NULL); 
                break;
            }
            site = site->next;
        }
    }

    pthread_mutex_unlock(&lock);
    return site;
}

void remove_cache_element() {
    pthread_mutex_lock(&lock);

    if (head != NULL) {
        cache_element *p = head;
        cache_element *q = head;
        cache_element *oldest = head;

        
        while (q->next != NULL) {
            if (q->next->lru_time_track < oldest->lru_time_track) {
                oldest = q->next;
                p = q;
            }
            q = q->next;
        }

        if (oldest == head) {
            head = head->next;
        } else {
            p->next = oldest->next;
        }

        cache_size -= oldest->len + sizeof(cache_element) + strlen(oldest->url) + 1;
        free(oldest->data);
        free(oldest->url);
        free(oldest);
    }

    pthread_mutex_unlock(&lock);
}

int add_cache_element(char* data, int size, char* url) {
    pthread_mutex_lock(&lock);

    int element_size = size + 1 + strlen(url) + sizeof(cache_element);
    if (element_size > MAX_ELEMENT_SIZE) {
        pthread_mutex_unlock(&lock);
        return 0;
    }

    
    while (cache_size + element_size > MAX_SIZE) {
        remove_cache_element();
    }

    cache_element* element = (cache_element*)malloc(sizeof(cache_element));
    if (!element) {
        pthread_mutex_unlock(&lock);
        return 0;
    }

    element->data = (char*)malloc(size + 1);
    if (!element->data) {
        free(element);
        pthread_mutex_unlock(&lock);
        return 0;
    }

    element->url = (char*)malloc(strlen(url) + 1);
    if (!element->url) {
        free(element->data);
        free(element);
        pthread_mutex_unlock(&lock);
        return 0;
    }

    strcpy(element->data, data);
    strcpy(element->url, url);
    element->len = size;
    element->lru_time_track = time(NULL);
    element->next = head;
    head = element;

    cache_size += element_size;

    pthread_mutex_unlock(&lock);
    return 1;
}
