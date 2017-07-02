#include "hashtable.h"
#include "md5.h"
#include "argparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OP_INSERT 0xA
#define OP_SEARCH 0xB
#define OP_REMOVE 0xC
#define ERR_NONE_SELECTED 0x1
#define ERR_MISS_PARAM 0x2
#define TABLE_SIZE 101 // nr primo

typedef struct argparse_option ArgsOpts;
typedef struct argparse Args;

static const char *const usage[] = {
    "cache-manager [options]",
    NULL,
};

/*Globals*/
int verbose = 0;
HashTable *table;
FILE *db;

void onInsertSelected(const char *key);
void onRemoveSelected(const char *key);
void onSearchSelected(const char *key);
void onNoneSelected();

void loadIndex(const char *indexPath);
void storeIndex(const char *indexPath);
void loadDb(const char *storePath);

int parseOperation(int insert, int remove, int search);

int main(int argc, const char **argv)
{
    int insert = 0;
    int remove = 0;
    int search = 0;
    int operation = 0;

    const char *key = NULL;
    const char *indexPath = NULL;
    const char *storePath = NULL;

    ArgsOpts options[] = {
        OPT_HELP(),
        OPT_GROUP("Opções básicas"),
        OPT_BOOLEAN('v', "verbose", &verbose, "Modo verboso"),
        OPT_STRING('t', "table", &indexPath, "Arquivo de indice"),
        OPT_STRING('d', "db", &storePath, "Arquivo de armazenamento"),
        OPT_STRING('k', "key", &key, "Chave de indexação"),
        OPT_BOOLEAN('i', "insert", &insert, "Operação de inserção"),
        OPT_BOOLEAN('r', "remove", &remove, "Operação de remoção"),
        OPT_BOOLEAN('s', "search", &search, "Operação de busca"),
        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nControle de cache de paginas web com Hash", "\nEste programa tem por objetivo interagir com um arquivo de tabela hash para gerenciar um cache web");
    argc = argparse_parse(&argparse, argc, argv);

    operation = parseOperation(insert, remove, search);

    /*Validação de parâmetros obrigatorios*/
    int hasParams = 1;
    if (key == NULL)
    {
        printf("Parametro de chave é obrigatório, use -k ou --key\n");
        hasParams = 0;
    }

    if (indexPath == NULL)
    {
        printf("Parametro de arquivo da tabela é obrigatório, use -t ou --table\n");
        hasParams = 0;
    }

    if (storePath == NULL)
    {
        printf("Parametro de arquivo de armazenamento é obrigatório, use -d ou --db\n");
        hasParams = 0;
    }
    if (!hasParams)
        return ERR_MISS_PARAM;

    loadIndex(indexPath);
    loadDb(storePath);

    switch (operation)
    {
    case OP_INSERT:
        onInsertSelected(key);
        break;
    case OP_REMOVE:
        onRemoveSelected(key);
        break;
    case OP_SEARCH:
        onSearchSelected(key);
        break;
    default:
        onNoneSelected();
        return ERR_NONE_SELECTED;
    }
    storeIndex(indexPath);
    return 0;
}

void onInsertSelected(const char *key)
{
    if (verbose)
    {
        printf("Iniciando operação de inserção na chave [%s]\n", key);
    }
}
void onRemoveSelected(const char *key)
{
    if (verbose)
    {
        printf("Iniciando operação de remoção da chave [%s]\n", key);
    }
}
void onSearchSelected(const char *key)
{
    if (verbose)
    {
        printf("Iniciando operação de busca da chave [%s]\n", key);
    }
}
void onNoneSelected()
{
    if (verbose)
    {
        printf("Nenhuma ou mais de uma opção selecionada\n");
    }
}

int parseOperation(int insert, int remove, int search)
{
    if (insert && !remove && !search)
    {
        return OP_INSERT;
    }
    if (!insert && remove && !search)
    {
        return OP_REMOVE;
    }
    if (!insert && !remove && search)
    {
        return OP_SEARCH;
    }
    return 0;
}
void loadIndex(const char *indexPath)
{
    if (indexPath != NULL)
    {
        table = hTableLoad(indexPath);
    }
    if (table == NULL)
    {
        table = hTableInit(TABLE_SIZE);
    }
}
void storeIndex(const char *indexPath)
{
}
void loadDb(const char *storePath)
{
}
char *str2md5(const char *str)
{
    int length = strlen(str);
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char *)malloc(33);

    MD5_Init(&c);

    while (length > 0)
    {
        if (length > 512)
        {
            MD5_Update(&c, str, 512);
        }
        else
        {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n)
    {
        snprintf(&(out[n * 2]), 16 * 2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}