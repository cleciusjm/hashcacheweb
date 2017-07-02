#include <stdio.h>

typedef struct
{
    char *key;
    int value;
} ValueEntry;

typedef struct valueEntryNode ValueEntryNode;

struct valueEntryNode
{
    ValueEntry *value;
    ValueEntryNode *next;
    ValueEntryNode *prev;
};

typedef struct
{
    ValueEntryNode **values;
    int verbose;
    int maxsize;
} HashTable;

ValueEntry *hTableInitEntry(const char *key, const unsigned long value);

HashTable *hTableInit(const unsigned long size);
/**
    Busca o valor da chave solicitada

    @params
        table - tabela a ser manipulada
        key - chave da tabela
    @returns 1 se remoção ok, 0 se não ok
**/
ValueEntry *hTableSearch(HashTable *table, const char *key);
/**
    Insere o valor na tabela

    @params
        table - tabela a ser manipulada
        value - valor a ser inserido
    @returns 1 se inserção ok, 0 se não ok
**/
int hTableInsert(HashTable *table, ValueEntry *value);
/**
    Remove o valor da chave

    @params
        table - tabela a ser manipulada
        key - chave da tabela
    @returns 1 se remoção ok, 0 se não ok
**/
int hTableRemove(HashTable *table, const char *key);

void hTableSave(HashTable *table, const char *filename);

HashTable *hTableLoad(const char *filename);
