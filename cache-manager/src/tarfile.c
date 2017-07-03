#include "tarfile.h"
#include <stdio.h>

TarFile tarLoad(const char *filename)
{
    return fopen(filename, "a+");
}

void tarRelease(TarFile file)
{
    fclose(file);
}

int tarWrite(TarFile file, const char *inputFile)
{
    fseek(file, 0L, SEEK_END);
    int tarIndex = ftell(file);
    FILE *in = fopen(inputFile, "r");
    if (in == NULL)
    {
        return TAR_NO_INDEX;
    }

    fseek(in, 0L, SEEK_END);
    int index = ftell(in);
    fseek(in, 0L, SEEK_SET);
    fwrite(&index, sizeof(int), 1, file);

    char buffer[1024 * 1024];
    size_t bytes;
    while (0 < (bytes = fread(buffer, 1, sizeof(buffer), in)))
        fwrite(buffer, 1, bytes, file);

    fseek(file, 0L, SEEK_SET);

    fclose(in);
    return tarIndex;
}
int tarReadTo(TarFile file, int index, FILE *out)
{
    fseek(file, index, SEEK_SET);
    int size = 0;
    fread(&size, sizeof(int), 1, file);

    char buffer[size];
    size_t bytes = fread(buffer, 1, sizeof(buffer), file);
    fwrite(buffer, 1, bytes, out);

    fseek(file, 0L, SEEK_SET);
    return 1;
}
int tarRead(TarFile file, int index, const char *outputFile)
{
    FILE *out = fopen(outputFile, "w");
    int result = tarReadTo(file, index, out);
    fclose(out);
    return result;
}
int tarReadToStdOut(TarFile file, int index)
{
    return tarReadTo(file, index, stdin);
}
