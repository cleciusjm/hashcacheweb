#include <stdio.h>
#define TAR_NO_INDEX -1

typedef FILE *TarFile;

TarFile tarLoad(const char *filename);

void tarRelease(TarFile file);

int tarWrite(TarFile file, const char *inputFile);

int tarRead(TarFile file, int index, const char *outputFile);

int tarReadToStdOut(TarFile file, int index);
