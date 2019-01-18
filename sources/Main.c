
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
    printf("./run -c file fileCompresso \n\n");
    printf("Commando di decompressione:\n");
    printf("./run -d fileCompresso fileDecompresso\n");
}


/**
 * Questa funzione serve per gestire i file di dimensione piccola, i file di dimensione inferiore
 * a MIN_SIZE_TO_COMPRESS bytes infatti non verranno compressi in quanto anche comprimendoli
 * il file compresso avrebbe una dimensione maggiore o uguale a quella del file in input.
 * @param fileSize: dimensione del file
 */
int exceptionManegement(unsigned int fileSize) {
    if (fileSize <= MIN_SIZE_TO_COMPRESS) {
        printf("File passato vuoto o piccolo per essere compresso\n");
        return 1;
    } else
        return 0;
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

        FILE *inputFile = fopen(argv[2], "rb");
        unsigned int fileSize = getFileSize(inputFile);
        if (exceptionManegement(fileSize)) {
            return 1;
        }

        printf("Inizio compressione LZS...\n");
        startLZS = clock();
        LZSCompression(inputFile, fileSize);
        printf("Fine compressione LZS\n\n");

        endLZS = clock();

        printf("Inizio compressione ShannonFano...\n");
        startSHF = clock();
        int bufferSize = getCodedBufferSize();
        unsigned char *buffer = getCodedBuffer();
        compressSHF(argv[3], buffer, bufferSize);
        endSHF = clock();
        printf("Fine compressione ShannonFano\n\n");

        executionTime(startLZS, endLZS, "LZS");
        executionTime(startSHF, endSHF, "SannonFano");
        executionTime(startLZS, endSHF, "LZS-SHF");

    } else if (strcmp(argv[1], "-d") == 0) {

        printf("Inizio decompressione ShannonFano...\n");
        startSHF = clock();
        unsigned char *buffer = decompressSHF(argv[2]);
        endSHF = clock();
        printf("Fine decompressione ShannonFano\n\n");
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
