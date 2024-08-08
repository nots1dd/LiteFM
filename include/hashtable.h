#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#define _POSIX_C_SOURCE 200809L
#define TABLE_SIZE 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define hash table entry structure
typedef struct Entry {
    char *key;
    struct Entry *next;
} Entry;

// Define hash table structure
typedef struct {
    Entry *entries[TABLE_SIZE];
} HashTable;

// Function prototypes
unsigned int hash(const char *key);
HashTable* create_table();
void free_table(HashTable *table);
void insert(HashTable *table, const char *key);
int search(HashTable *table, const char *key);
int hash_table_contains(HashTable *table, const char *key);
void print_table(HashTable *table);

#endif
