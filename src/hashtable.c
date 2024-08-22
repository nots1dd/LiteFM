#include "../include/hashtable.h"

// Hash function
unsigned int hash(const char* key)
{
  unsigned long int value   = 0;
  unsigned int      i       = 0;
  unsigned int      key_len = strlen(key);

  for (; i < key_len; ++i)
  {
    value = value * 37 + key[i];
  }

  value = value % TABLE_SIZE;

  return value;
}

// Create a new hash table
HashTable* create_table()
{
  HashTable* table = (HashTable*)malloc(sizeof(HashTable));

  for (int i = 0; i < TABLE_SIZE; ++i)
  {
    table->entries[i] = NULL;
  }

  return table;
}

// Free the hash table
void free_table(HashTable* table)
{
  for (int i = 0; i < TABLE_SIZE; ++i)
  {
    Entry* entry = table->entries[i];
    while (entry != NULL)
    {
      Entry* temp = entry;
      entry       = entry->next;
      free(temp->key);
      free(temp);
    }
  }
  free(table);
}

// Insert a key into the hash table
void insert(HashTable* table, const char* key)
{
  unsigned int slot = hash(key);

  Entry* entry = table->entries[slot];
  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      return;
    }
    entry = entry->next;
  }

  Entry* new_entry     = (Entry*)malloc(sizeof(Entry));
  new_entry->key       = strdup(key);
  new_entry->next      = table->entries[slot];
  table->entries[slot] = new_entry;
}

// Search for a key in the hash table
int search(HashTable* table, const char* key)
{
  unsigned int slot = hash(key);

  Entry* entry = table->entries[slot];
  while (entry != NULL)
  {
    if (strcmp(entry->key, key) == 0)
    {
      return 1;
    }
    entry = entry->next;
  }

  return 0;
}

// Check if hash table contains a character key
int hash_table_contains(HashTable* table, const char* key)
{
  char         key_str[2] = {*key, '\0'};
  unsigned int slot       = hash(key_str);
  Entry*       entry      = table->entries[slot];

  while (entry != NULL)
  {
    if (strcmp(entry->key, key_str) == 0)
    {
      return 1;
    }
    entry = entry->next;
  }
  return 0;
}

void print_table(HashTable* table)
{
  for (int i = 0; i < 1000; ++i)
  {
    Entry* entry = table->entries[i];
    if (entry != NULL)
    {
      while (entry != NULL)
      {
        printf("  %s\n", entry->key);
        entry = entry->next;
      }
    }
  }
  printf("\n");
}
