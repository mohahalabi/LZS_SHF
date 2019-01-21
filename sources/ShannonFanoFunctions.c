/*
Shannon-Fano's Functions implementation
****************************************************************************
Author: Mohammad Halabi
Programming language: C standard version 99
****************************************************************************
*/

#include "../headers/ShannonFanoFunctions.h"
#include "../headers/LZSFunctions.h"

/************************ Compression's Functions Definition *****************************/


void initializeElements(Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].frequency = 0;
        ptrElements[i].codeLength = 0;
    }
}


/**
 * Description: calcola le frequenze di bytes presenti
 *
 * @param
 * ptrElements: tabella di frequenze
 * buffer: puntatore al buffer che contiene la codifica di LZS
 * bufferSize: size di buffer
 * */
void calculateFrequencies(Element *ptrElements, unsigned char *buffer, int bufferSize) {
    unsigned char ch;
    int index = 0;
    while (index < bufferSize) {
        ch = buffer[index];
        ptrElements[ch].frequency++;
        index++;
    }
}


int compareByFreqStabilized(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    if (element1->frequency < element2->frequency) {
        return 1;
    } else if (element1->frequency > element2->frequency) {
        return -1;
    } else {  // if frequency1 = frequency2 order by word
        return element1->word - element2->word;
    }
}


int compareByCodeLengthStabilized(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    int length1 = element1->codeLength;
    int length2 = element2->codeLength;
    if (length1 < length2) {
        return -1;
    } else if (length1 > length2) {
        return 1;
    } else {  // if length1 = length2 order by word
        return element1->word - element2->word;
    }
}


int compareByWord(const void *a, const void *b) {
    Element *element1 = (Element *) a;
    Element *element2 = (Element *) b;
    return element1->word - element2->word;
}


void orderByFreqDesc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByFreqStabilized);
}


void orderBycodeLengthCresc(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByCodeLengthStabilized);
}


void orderByWord(Element *ptrElements) {
    qsort(ptrElements, MAX_CODE, sizeof(Element), compareByWord);
}


/**
 * Description: funzione che restituisce la somma delle frequenze
 *              presenti nel sotto albero del nodo passato, la funzione verrà
 *              chiamata nella funzione getSplitIndex
 * @param
 * ptrElements: tabella di elementi
 * root: il nodo padre di cui si vuole sapere le somma di frequenze che copre
 * */
long long int sumFrequencies(Element *ptrElements, Node *root) {
    long long int sumOfFrequencies = 0;
    for (int i = root->start; i <= root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + ptrElements[i].frequency;
    }
    return sumOfFrequencies;
}

/**
 * Description: funzione che restituisce l'indice in cui avviene la suddivisione
 *              esempio : se abbiamo il seguente tabella
 *              index   0 1 2 3 4 5
 *              word    a b c d e f
 *              freq    2 2 1 1 1 1
 *              La funzione restituisce l'indice di suddivisione '1'
 *              tale suddivisione produce due nodi figli;
 *              sinistro:a b    e destro: c d e f
 * @param
 * ptrElements: tabella di elementi
 * root: il nodo padre di cui si vuole sapere le somme di frequenze che copre
 * */
int getSplitIndex(Element *ptrElements, Node *root) {
    long long int sumOfFrequencies = sumFrequencies(ptrElements, root);
    int splitIndex = 0;
    double halfOfSum;
    long sum = 0;
    halfOfSum = 1.0 * sumOfFrequencies / 2;
    int from = root->start;
    int to = root->end;
    for (int i = from; i < to; i++) {
        sum = sum + ptrElements[i].frequency;
        if (sum + ptrElements[i + 1].frequency >= halfOfSum) {
            splitIndex = i;
            return splitIndex;
        }
    }
    return splitIndex;
}

/**
 * Description: crea e restituisce un puntatore a un nuovo nodo
 *
 * @param
 * lNode: puntatore al figlio sinistro
 * rNode: puntatore al figlio destro
 * start, end indicano i sotto nodi coperti dal nodo restituito
 * */
Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *newNode = malloc(sizeof(Node));
    newNode->leftChild = lNode;
    newNode->rightChild = rNode;
    newNode->start = start;
    newNode->end = end;
    return newNode;
}

/**
 * Description: crea l'albero di codifica, questa funzione viene chiamata
 *              in modo ricorsivo
 *
 * @param
 * ptrElements : tabella degli elementi
 * root: radice dell'albero
 * */
void createEncodingTree(Element *ptrElements, Node *root) {
    int splitIndex = getSplitIndex(ptrElements, root);

    if (root->start == root->end) {
        return;
    }
    if (root->start == (root->end - 1)) {
        Node *leftNode = createNode(NULL, NULL, root->start, root->start);
        Node *rightNode = createNode(NULL, NULL, root->end, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
    } else {
        Node *leftNode = createNode(NULL, NULL, root->start, splitIndex);
        Node *rightNode = createNode(NULL, NULL, splitIndex + 1, root->end);
        root->leftChild = leftNode;
        root->rightChild = rightNode;
        createEncodingTree(ptrElements, root->leftChild);
        createEncodingTree(ptrElements, root->rightChild);
    }
}

// variabile globale per immagazzinare le codifiche in modo temporaneo
char charactersHolder[MAX_CODE] = "";

/**
 * Description: procedura per creare le codifiche partendo dal albero
 *              creato in precedenza
 *
 * @param
 * ptrCodes: puntatore alle codifiche temporanee
 * root: radice dell'albero
 * */
void encode(Code *ptrCodes, Node *root) {
    if (root->leftChild != NULL) {
        strcat(charactersHolder, "0");
        encode(ptrCodes, root->leftChild);
        charactersHolder[strlen(charactersHolder) - 1] = '\0';
        strcat(charactersHolder, "1");
        encode(ptrCodes, root->rightChild);
        charactersHolder[strlen(charactersHolder) - 1] = '\0';
    } else {
        strcpy(ptrCodes[root->start].code, charactersHolder);
    }
}

/**
 * Description: procedura per copiare le codifiche dalla struttura temporanea
 *              alla tabella degli elementi
 *              questa operazione viene eseguita solo per gli elementi che hanno
 *              una frequenza > 0, quindi presenti nel file di input
 *
 * @param
 * ptrElements: tabella degli elementi
 * ptrCodes: puntatore alle codifiche temporanee
 * */
void writeCodesForAllElements(Element *ptrElements, Code *ptrCodes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        if (ptrElements[targetIndex].frequency > 0) {
            strcpy(ptrElements[targetIndex].code, ptrCodes[targetIndex].code);
            ptrElements[targetIndex].codeLength = (unsigned char) strlen(ptrElements[targetIndex].code);
        }
    }
}

/**
 * Description: funzione che restituisce un puntatore alla struttura CodeBits
 *                 che verrà utilizzata per generare la codifica canonica
 *
 * @param
 * 
 * */
CodeBits *getCodeBits() {
    CodeBits *codeBits = malloc(sizeof(CodeBits));
    codeBits->bits = 0;
    return codeBits;
}

/**
 * Description: dato un numero a 64 bits, questa funzione converte i bits,
 *              per una lunghezza data (la lunghezza della codifica) in
 *              caratteri (zeri ed uni) e restituisce un puntatore a tali caratteri
 *
 * @param
 * num: il numero da convertire
 * length: la lunghezza della codifica
 * */
char *fromNumToChars(long long num, int length) {
    char *code = malloc(length * sizeof(char));
    strcpy(code, "");
    for (int i = length - 1; i >= 0; i--) {
        (num & (1 << i)) ? strcat(code, "1") : strcat(code, "0");
    }
    return code;
}


/**
 * Description: Procedura che genera la codifica canonica di tutti gli elementi
 *              presenti nei dati di input (il file o il buffer di LZS)
 *              le codifiche precedenti generati con l'albero di shannon fano vengono
 *              sostituiti con la codifica canonica.
 *              Gli elementi vengono passati ordinati per lunghezze crescenti.
 *              Questa procedura va sfruttata sia in compressione che in decompressione
 *
 * @param
 * ptrElements: tabella degli elementi
 * */
void canonizeCodes(Element *ptrElements) {

    CodeBits *codeBits = getCodeBits();
    int firstLength = ptrElements[0].codeLength;   // è la lunghezza minore
    for (int i = 0; i < firstLength; ++i) {
        ptrElements[0].code[i] = '0';   // riempire con zeri
    }
    int currentLength = 0;
    for (int i = 1; i < MAX_CODE; ++i) {
        currentLength = ptrElements[i].codeLength; // la lunghezza di codifica subito dopo
        if (currentLength == firstLength) { // se sono uguali, sommare uno
            codeBits->bits++;
            strcpy(ptrElements[i].code, fromNumToChars(codeBits->bits, currentLength));
        } else if (currentLength > firstLength) {   // altrimenti sommare uno e shiftare a sinistra
            codeBits->bits++;                       // quanto la differenza tra le due lunghezze
            for (int j = 0; j < (currentLength - firstLength); ++j) {
                codeBits->bits = codeBits->bits << 1;
            }
            strcpy(ptrElements[i].code, fromNumToChars(codeBits->bits, currentLength));
        }
        firstLength = currentLength;    // aggiornare le lunghezze
    }
}


long bitsNumber = 0; // Tutti i bits scritti sul file compresso
unsigned char byte = 0;
int currentBit = 0; // il numero di bit currente da scrivere, quando è uguale a 8, lo rimetto a 0

void addBitToFile(unsigned char bit, FILE *file) {
    currentBit++;
    byte = byte << 1 | bit;
    if (currentBit == 8) {
        writeByte(file);
        currentBit = 0;
        byte = 0;
    }
}

void writeByte(FILE *file) {
    fputc(byte, file);
}


/**
 * Description: procedura per scrivere il blocco di 256 lunghezze
 *              sul file compresso
 *
 * @param
 * outputFile : puntatore al file di output (file compresso)
 * ptrElements: tabella degli elementi
 * */
void writeLengths(FILE *outputFile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        lengths[targetIndex] = ptrElements[i].codeLength;
    }
    fwrite(lengths, sizeof(unsigned char), MAX_CODE, outputFile);
    bitsNumber += (MAX_CODE * 8);
}

/**
 * Description: scrittura finale del file compresso
 *
 * @param
 * buffer: puntatore al buffer passato da LZS
 * bufferSize: size del buffer
 * outputFile : puntatore al file di output (file compresso)
 * ptrElements: tabella degli elementi
 * */
void writeCompressedFile(unsigned char *buffer, int bufferSize, FILE *outputFile, Element *ptrElements) {

    fseek(outputFile, 1, SEEK_SET); // lascio spazio per il byte di flag!
    writeLengths(outputFile, ptrElements);  // scrivo le lunghezze

    fseek(outputFile, MAX_CODE + 1, SEEK_SET);

    unsigned char ch;
    int index = 0;
    while (index < bufferSize) {
        ch = buffer[index];
        for (int i = 0; i < ptrElements[ch].codeLength; ++i) {
            char bit = ptrElements[ch].code[i];
            bit == '0' ? addBitToFile(0, outputFile) : addBitToFile(1, outputFile);
        }
        bitsNumber += ptrElements[ch].codeLength;
        index++;
    }
    int lastBitsToWrite = 8 - (bitsNumber % 8); // scrittura degli ultimi bits

    for (int i = 0; i < lastBitsToWrite; ++i) { // completare l'ultimo byte con zeri
        addBitToFile(0, outputFile);
        bitsNumber++;
    }
    // scrivo il byte di flag
    fseek(outputFile, 0, SEEK_SET);
    fputc(lastBitsToWrite, outputFile);
    bitsNumber += 8;
    fclose(outputFile);
}


/**
 * Description: La procedura principale tramite la quale vengono chiamate
 *              e gestite tutte le funzione/procedure legate alla compressione.
 *              Si occupa anche della allocazione della memoria necessaria per
 *              il processo di compressione
 *
 * @param
 * buffer: puntatore al buffer passato da LZS
 * bufferSize: size del buffer
 * outputFile : puntatore al file di output (file compresso)
 * */
void compressSHF(char *toCompFileName, unsigned char *codedBuffer, int codedBufferCounter) {

    Element *elements = malloc(MAX_CODE * sizeof(Element));
    Code *codes = malloc(MAX_CODE * sizeof(Code));

    initializeElements(elements);
    calculateFrequencies(elements, codedBuffer, codedBufferCounter);

    orderByFreqDesc(elements);
    Node *root = createNode(NULL, NULL, 0, MAX_CODE - 1);
    createEncodingTree(elements, root);

    encode(codes, root);
    writeCodesForAllElements(elements, codes);

    free(codes);
    free(root);

    orderBycodeLengthCresc(elements);
    canonizeCodes(elements);

    orderByWord(elements);
    FILE *compressed = fopen(toCompFileName, "wb");

    writeCompressedFile(codedBuffer, codedBufferCounter, compressed, elements);
    free(codedBuffer);
}


/************************ Decompression's Functions Definition *****************************/

int lastBits = 0;

// Il header include 1 byte di flag e 256 bytes di lunghezze
void readHeader(FILE *inputFile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    fseek(inputFile, 0, SEEK_SET);
    fread(&lastBits, sizeof(unsigned char), 1, inputFile); // leggere il byte di flag
    fread(lengths, sizeof(unsigned char), MAX_CODE, inputFile); // leggere le lunghezze

    // per ogni elemento, assegnare la respettiva lunghezza
    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].codeLength = lengths[i];
    }
}


/**
 * Description: leggere il file compresso e copiarlo in memoria
 *
 * @param
 * inputFile: puntatore al file compresso
 * buffer: puntatore ai dati letti dal file
 * bufferSize: dimensione del buffer, è uguale alla dimensione del file
 * */
void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer) {
    fseek(inputFile, MAX_CODE + 1, SEEK_SET);
    fread(buffer, sizeof(unsigned char), bufferSize, inputFile);
}


/**
 * Description: procedura ricorsiva per creare l'albero di decodifica
 *
 * @param
 * root: nodo radice, cambia con ogni chiamata ricorsiva
 * code: la codofica
 * index: indice di scorrimento della codifica
 * word: il valore da assegnare al giusto nodo
 * */
void createDecodingTree(Node *root, const char *code, int index, unsigned char word) {
    if (code[index] != '\0') { // se la codifica contiene ancora dei bits
        if (code[index] == '0') {   // se il bit è un '0'
            if (root->leftChild == NULL) { // ed il figlio sinistro == NULL
                Node *lNode = createNode(NULL, NULL, -1, -1);  // creare un nuovo nodo
                root->leftChild = lNode;   // assegnare il nuovo nodo al figlio sinistro
                index++;    // incrementa l'indice di bit
                createDecodingTree(root->leftChild, code, index, word); // fare la stessa cosa con il prossimo bit
            } else {  // se il figlio sinistro non è NULL
                index++;
                createDecodingTree(root->leftChild, code, index, word); // scendi nel figlio sinistro
            }
        } else { // altrimenti il bit è un '1'
            if (root->rightChild == NULL) {
                Node *rNode = createNode(NULL, NULL, -1, -1);
                root->rightChild = rNode;
                index++;
                createDecodingTree(root->rightChild, code, index, word);
            } else {
                index++;
                createDecodingTree(root->rightChild, code, index, word);
            }
        }
    } else {  // quando sono esauriti i bits della codifica, assegnare il valore della codifica al nodo
        root->start = word;
        root->end = word;
        return;
    }
}


/**
 * Description: Procedura chiamante di createDecodingTree.
 *              Passa le codifiche di tutti gli elementi alla procedura createDecodingTree
 *              per inizializzare i nodi nelle posizioni giuste.
 *
 * @param
 * root: nodo radice
 * ptrElements: tabella degli elementi
 * */
void decode(Node *root, Element *ptrElements) {
    for (int i = 0; i < MAX_CODE; ++i) {
        if (ptrElements[i].codeLength != 0) {
            createDecodingTree(root, ptrElements[i].code, 0, ptrElements[i].word);
        }
    }
}


// 1 <= bit position <= 8
int extractBitOnPosition(unsigned char byte, int bitPosition) {
    return (byte & (1 << (8 - bitPosition)) ? '1' : '0');
}


/**
 * Description: Dato un byte, lo restituisce in forma di caratteri zeri ed uni
 *
 * @param
 * byte: il byte da convertire in caratteri
 * */
char *byteToChars(unsigned char byte) {
    char *chars = malloc(8 * sizeof(char));
    for (int i = 1; i <= 8; ++i) {
        chars[i - 1] = (char) extractBitOnPosition(byte, i);
    }
    return chars;
}


/**
 * Description: Questa funzione ha il compito di attraversare l'albero di decodifica
 *              per identificare la codifica originale per ogni byte.
 *              La funzione restituisce alla fine un puntatore ai dati che saranno decompressi
 *              dall'algoritmo LZS.
 *
 * @param
 * root: radice dell'albero di decodifica
 * bufferIn: puntatore ai dati compressi
 * bufferSize: size di bufferIn
 * */
unsigned char *writeDecompBuffer(Node *root, unsigned char *bufferIn, int bufferSize) {

    Node *actualRoot = root;  // radice effettivo
    unsigned int initialBuffSize = 512; // 512 bytes: dimensione iniziale del buffer
    unsigned char *bufferOut = malloc(initialBuffSize * sizeof(unsigned char));

    unsigned int indexBuffIn = 0;   // indice di scorrimento del buffer in entrata
    unsigned int indexBuffOut = 0;  // indice di scorrimento del buffer in uscita

    while (indexBuffIn < bufferSize) {
        char *byte = byteToChars(bufferIn[indexBuffIn]);
        for (int i = 0; i < 8; ++i) {

            if (indexBuffIn == bufferSize - 1 && i == (8 - lastBits)) { // siamo arrivati all'ultimo byte?
                break; // allora trascurare gli ultimi bit che non fanno parte del file originale
            }
            if (byte[i] == '0')
                root = root->leftChild;
            else
                root = root->rightChild;

            if (root->leftChild == NULL) { // sono in una foglia
                if (indexBuffOut == initialBuffSize - 1) { // controllo se lo spazio allocato è esaurito
                    // raddoppio la dimensione di spazio iniziale
                    initialBuffSize <<= 1;
                    // realloco la memoria estesa
                    bufferOut = realloc(bufferOut, initialBuffSize * sizeof(unsigned char));
                }
                bufferOut[indexBuffOut] = (unsigned char) root->start; // scrivo il byte originale
                indexBuffOut++;
                root = actualRoot;
            }
        }
        indexBuffIn++;
        free(byte);
    }
    return bufferOut; // restituisco il puntatore a LZS
}


/**
 * Description: La procedura principale tramite la quale vengono chiamate
 *              e gestite tutte le funzione/procedure legate alla decompressione.
 *              Si occupa anche della allocazione della memoria necessaria per
 *              il processo di deccompressione
 *
 * @param
 * compFileName: il nome del file compresso passato dall'utente
 * */
unsigned char *decompressSHF(char *compFileName) {

    Element *elements = malloc(MAX_CODE * sizeof(Element));
    Node *root = createNode(NULL, NULL, -1, -1);
    FILE *compressed = fopen(compFileName, "rb");

    unsigned int fileSize = getFileSize(compressed);
    unsigned int bufferSize = fileSize - (MAX_CODE + 1);  // size of header = MAX_CODE + 1 = 257

    readHeader(compressed, elements);

    orderBycodeLengthCresc(elements);
    canonizeCodes(elements);

    unsigned char *buffer = malloc(bufferSize * sizeof(unsigned char));
    readCompressedFile(compressed, bufferSize, buffer);

    decode(root, elements);

    unsigned char *decompBuffer = writeDecompBuffer(root, buffer, bufferSize);

    free(buffer);
    free(root);
    free(elements);

    return decompBuffer;
}
