
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
    printf("Invalid argoments!\n");
    printf("Compression's command:\n");
    printf("executableFile -c fileToCompress compressedFileName\n\n");
    printf("Decompression's command:\n");
    printf("executableFile -d compressedFileName decompressedFileName \n");
}


int isInvalidArgoments(int argc) {
    if (argc != 4) {
        argsErrorMessage();
        return 1;
    }
    return 0;
}


int isInvalidFile(char *inputFile) {
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        printf("The file does not exist!\n");
        return 1;
    }
    fclose(file);
    return 0;
}


int main(int argc, char *argv[]) {

    if (isInvalidArgoments(argc) || isInvalidFile(argv[2])) {
        return 1;
    }
    if (strcmp(argv[1], "-c") == 0) {
        printf("inizio compressione LZS...\n\n");
        clock_t inizio = clock();
        int error = LZSCompression(argc, argv[2]);
        if (error)
            return 1;
        printf("fine compressione LZS\n\n");
        printf("inizio compressione shannonFano...\n\n");
        int bufferSize = getCodedBufferSize();
        unsigned char *buffer = getCodedBuffer();
        compressSHF(argv[3], buffer, bufferSize);
        printf("fine compressione shannonFano\n\n");
        clock_t fine = clock();
        double tempo = (double) (fine - inizio) / CLOCKS_PER_SEC;
        printf("Il tempo di compressione e' pari a: %lf secondi\n\n", tempo);

    } else if (strcmp(argv[1], "-d") == 0) {
        clock_t inizio = clock();
        printf("inizio decompressione shannonFano...\n\n");
        unsigned char *buffer = decompressSHF(argv[2]);
        printf("fine decompressione shannon\n\n");

        printf("inizio decompressione LZS...\n\n");
        LZSDecompression(argc, buffer, argv[3]);
        printf("fine decompressione LZS\n\n");

        clock_t fine = clock();
        double tempo = (double) (fine - inizio) / CLOCKS_PER_SEC;
        printf("Il tempo di decompressione e' pari a: %lf secondi\n\n", tempo);
    } else {
        printf("errore negli argomenti passati \n");
        return 1;
    }
    return 0;
}
