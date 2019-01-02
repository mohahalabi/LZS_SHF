#include "../headers/ShannonFanoFunctions.h"
#include "../headers/LZSFunctions.h"


int main(int argc, char *argv[]) {
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
        compress(argv[3], buffer, bufferSize);
        printf("fine compressione shannonFano\n\n");
        clock_t fine = clock();
        double tempo = (double) (fine - inizio) / CLOCKS_PER_SEC;
        printf("Il tempo di compressione e' pari a: %lf secondi", tempo);

    } else if (strcmp(argv[1], "-d") == 0) {
        clock_t inizio = clock();
        printf("inizio decompressione shannonFano...\n\n");
        unsigned char *buffer = decompress(argv[2]);
        printf("fine decompressione shannon\n\n");

        printf("inizio decompressione LZS...\n\n");
        LZSDecompression(argc, buffer, argv[3]);
        printf("fine decompressione LZS\n\n");

        clock_t fine = clock();
        double tempo = (double) (fine - inizio) / CLOCKS_PER_SEC;
        printf("Il tempo di decompressione e' pari a: %lf secondi", tempo);
    } else {
        printf("errore negli argomenti passati \n");
        return 1;
    }
    return 0;
}
