#ifndef CACHE_H
#define CACHE_H

#include <time.h>

// Struct definition for a cache element
typedef struct cache_element {
    char* data;                 // Cached response data
    int len;                    // Length of the data
    char* url;                  // Request URL (used as key)
    time_t lru_time_track;      // Timestamp for LRU eviction
    struct cache_element* next;
} cache_element;

/**
 * Initializes cache data structures and lock.
 * Must be called before using any cache operations.
 */
void init_cache();

/**
 * Looks up a response in the cache by URL.
 * Returns pointer to cache_element if found, NULL otherwise.
 */
cache_element* find(char* url);

/**
 * Adds a new response to the cache for the given URL.
 * Performs LRU eviction if needed.
 * Returns 1 on success, 0 on failure.
 */
int add_cache_element(char* data, int size, char* url);

/**
 * Removes the least recently used (LRU) cache element.
 * Called internally when space needs to be freed.
 */
void remove_cache_element();

#endif // CACHE_H
