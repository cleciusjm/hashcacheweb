#include "hashtable.h"
#include "tarfile.h"
#include "md5.h"
#include "argparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OP_INSERT 0xA
#define OP_SEARCH 0xB
#define OP_REMOVE 0xC
#define STAT_OK 0x0
#define ERR_NONE_SELECTED 0x1
#define ERR_MISS_PARAM 0x2
#define ERR_FAIL_TO_EXECUTE 0x3
#define ERR_NOT_FOUND 0x4
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
TarFile db;

int onInsertSelected(const char *key, const char *inputFile);
int onRemoveSelected(const char *key);
int onSearchSelected(const char *key, const char *outDir);
int onNoneSelected();

void loadIndex(const char *indexPath);
void storeIndex(const char *indexPath);
void loadDb(const char *storePath);

int parseOperation(int insert, int remove, int search);
char *str2md5(const char *str);
int main(int argc, const char **argv)
{
    int insert = 0;
    int remove = 0;
    int search = 0;
    int operation = 0;

    char *key = NULL;
    const char *indexPath = NULL;
    const char *storePath = NULL;
    char *out = NULL;
    char *in = NULL;
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
        OPT_STRING('I', "in", &in, "Arquivo de entrada para inserção"),
        OPT_STRING('O', "out", &out, "Arquivo de saída para busca"),
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

    key = str2md5(key);
    loadIndex(indexPath);
    loadDb(storePath);
    table->verbose = verbose;
    if (out == NULL)
    {
        char *extension = ".html";
        out = malloc(sizeof(char) * (strlen(key)) + strlen(extension));
        strcpy(out, key);
        strcat(out, extension);
    }
    int status = STAT_OK;
    switch (operation)
    {
    case OP_INSERT:
        status = onInsertSelected(key, in);
        break;
    case OP_REMOVE:
        status = onRemoveSelected(key);
        break;
    case OP_SEARCH:
        status = onSearchSelected(key, out);
        break;
    default:
        status = onNoneSelected();
        break;
    }
    storeIndex(indexPath);
    return status;
}

int onInsertSelected(const char *key, const char *inputFile)
{
    if (verbose)
        printf("Iniciando operação de inserção na chave [%s] do arquivo [%s]\n", key, inputFile);

    /*Inserção do DB*/
    int tarIndex = TAR_NO_INDEX;
    if (inputFile == NULL)
    {
        if (verbose)
            printf("Arquivo de entrada não informado, utilize a opção -I ou --in \n");
        return ERR_MISS_PARAM;
    }
    tarIndex = tarWrite(db, inputFile);
    if (tarIndex == TAR_NO_INDEX)
    {
        if (verbose)
            printf("Falha ao ler o arquivo informado \n");
        return ERR_FAIL_TO_EXECUTE;
    }

    /*Inserção no hash*/
    ValueEntry *entry = hTableInitEntry(key, tarIndex);
    if (hTableInsert(table, entry))
    {
        if (verbose)
            printf("Inserido com sucesso!\n");
        return STAT_OK;
    }
    else
    {
        if (verbose)
            printf("Falha ao inserir!\n");
        return ERR_FAIL_TO_EXECUTE;
    }
}
int onRemoveSelected(const char *key)
{
    if (verbose)
        printf("Iniciando operação de remoção da chave [%s]\n", key);
    if (hTableRemove(table, key))
    {
        if (verbose)
            printf("Removido com sucesso!\n");
        return STAT_OK;
    }
    else
    {
        if (verbose)
            printf("Falha ao remover!\n");
        return ERR_FAIL_TO_EXECUTE;
    }
}

int onSearchSelected(const char *key, const char *out)
{
    if (verbose)
        printf("Iniciando operação de busca da chave [%s]\n", key);
    ValueEntry *result = hTableSearch(table, key);
    if (result == NULL)
    {
        if (verbose)
            printf("Chave solicitada não existe na tabela\n");
        return ERR_NOT_FOUND;
    }
    else
    {
        int toDefaultOut = strcmp(out, "-") == 0;
        if (verbose)
            printf("Chave encontrada, posição [%d], enviando resultado para [%s]\n", result->value, toDefaultOut ? "Saída Padrão" : out);

        if (toDefaultOut)
            tarReadToStdOut(db, result->value);
        else
            tarRead(db, result->value, out);

        return STAT_OK;
    }
}

int onNoneSelected()
{
    if (verbose)
        printf("Nenhuma ou mais de uma opção selecionada\n");
    return ERR_NONE_SELECTED;
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
        table = hTableLoad(indexPath, verbose);
    }
    if (table == NULL)
    {
        table = hTableInit(TABLE_SIZE);
    }
}
void storeIndex(const char *indexPath)
{
    if (table != NULL)
    {
        hTableSave(table, indexPath);
    }
}
void loadDb(const char *storePath)
{
    db = tarLoad(storePath);
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