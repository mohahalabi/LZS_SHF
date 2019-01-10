
/*
LZS - Shannon-Fano Compression algorithm.
****************************************************************************
Authors
LZS: Eric Palamas
Shannon-Fano: Mohammad Halabi

Programming language: C standard version 99
****************************************************************************
*/

#include "../headers/ShannonFanoFunctions.h"
#include "../headers/LZSFunctions.h"


void argsErrorMessage() {
    printf("Gli argomenti non sono validi!\n");
    printf("Commando di compressione:\n");
    printf("./eseguibile -c file fileCompresso \n\n");
    printf("Commando di decompressione:\n");
    printf("./eseguibile -d fileCompresso fileDecompresso\n");
}


int isInvalidArgs(int argc) {
    if (argc != 4) {
        argsErrorMessage();
        return 1;
    }
    return 0;
}


int isInvalidFile(char *inputFile) {
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        printf("Il file non esiste!\n");
        return 1;
    }
    fclose(file);
    return 0;
}


void executionTime(clock_t start, clock_t end, char *algorithm) {
    double tempo = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Tempo d'esecuzione di %s: %.3lf secondi\n", algorithm, tempo);
}


int main(int argc, char *argv[]) {

    clock_t startSHF;
    clock_t endSHF;

    clock_t startLZS;
    clock_t endLZS;

    if (isInvalidArgs(argc) || isInvalidFile(argv[2])) {
        return 1;
    }
    if (strcmp(argv[1], "-c") == 0) {

        printf("Inizio compressione LZS...\n");
        startLZS = clock();
        int error = LZSCompression(argc, argv[2]);
        if (error)
            return 1;
        printf("Fine compressione LZS\n\n");

        endLZS = clock();

        printf("Inizio compressione ShannonFano...\n");
        startSHF = clock();
        int bufferSize = getCodedBufferSize();
        unsigned char *buffer = getCodedBuffer();
        compressSHF(argv[3], buffer, bufferSize);
        endSHF = clock();
        printf("Fine compressione shannonFano\n\n");

        executionTime(startLZS, endLZS, "LZS");
        executionTime(startSHF, endSHF, "SannonFano");
        executionTime(startLZS, endSHF, "LZS-SHF");

    } else if (strcmp(argv[1], "-d") == 0) {

        printf("Inizio decompressione shannonFano...\n");
        startSHF = clock();
        unsigned char *buffer = decompressSHF(argv[2]);
        endSHF = clock();
        printf("Fine decompressione shannon\n\n");
        printf("Inizio decompressione LZS...\n");
        startLZS = clock();
        LZSDecompression(argc, buffer, argv[3]);
        endLZS = clock();
        printf("Fine decompressione LZS\n\n");

        executionTime(startSHF, endSHF, "SannonFano");
        executionTime(startLZS, endLZS, "LZS");
        executionTime(startSHF, endLZS, "LZS-SHF");
    } else {
        argsErrorMessage();
        return 1;
    }
    return 0;
}
