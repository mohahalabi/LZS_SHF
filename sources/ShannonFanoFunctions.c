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
    } else {  // if zero order by word
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
    } else {  // if zero order by word
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


long long int sumFrequencies(Element *ptrElements, Node *root) {
    long long int sumOfFrequencies = 0;
    for (int i = root->start; i <= root->end; ++i) {
        sumOfFrequencies = sumOfFrequencies + ptrElements[i].frequency;
    }
    return sumOfFrequencies;
}


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


Node *createNode(Node *lNode, Node *rNode, int start, int end) {
    Node *newNode = malloc(sizeof(Node));
    newNode->leftChild = lNode;
    newNode->rightChild = rNode;
    newNode->start = start;
    newNode->end = end;
    return newNode;
}


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


char charactersHolder[MAX_CODE] = "";

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


void writeCodesForAllElements(Element *ptrElements, Code *ptrCodes) {
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        if (ptrElements[targetIndex].frequency > 0) {
            strcpy(ptrElements[targetIndex].code, ptrCodes[targetIndex].code);
            ptrElements[targetIndex].codeLength = (unsigned char) strlen(ptrElements[targetIndex].code);
        }
    }
}


CodeBits *getCodeBits() {
    CodeBits *codeBits = malloc(sizeof(CodeBits));
    codeBits->bits = 0;
    return codeBits;
}


char *fromNumToChars(long long num, int length) {
    char *code = malloc(length * sizeof(char));
    strcpy(code, "");
    for (int i = length - 1; i >= 0; i--) {
        (num & (1 << i)) ? strcat(code, "1") : strcat(code, "0");
    }
    return code;
}

// funzionante per lunghezze di codifiche minori di 64 bits
void canonizeCodes(Element *ptrElements) {

    CodeBits *codeBits = getCodeBits();
    int firstLength = ptrElements[0].codeLength;
    for (int i = 0; i < firstLength; ++i) {
        ptrElements[0].code[i] = '0';
    }
    int currentLength = 0;
    for (int i = 1; i < MAX_CODE; ++i) {
        currentLength = ptrElements[i].codeLength;
        if (currentLength == firstLength) {
            codeBits->bits++;
            strcpy(ptrElements[i].code, fromNumToChars(codeBits->bits, currentLength));
        } else if (currentLength > firstLength) {
            codeBits->bits++;
            for (int j = 0; j < (currentLength - firstLength); ++j) {
                codeBits->bits = codeBits->bits << 1;
            }
            strcpy(ptrElements[i].code, fromNumToChars(codeBits->bits, currentLength));
        }
        firstLength = currentLength;
    }
}


long bitsNumber = 0;    // All the bits written on the compressed file
unsigned char byte = 0; // Single byte to write on the compressed file
int currentBit = 0;     // The current bit to add, when its value is 8, I reset it to 0

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


void writeLengths(FILE *outputFile, Element *ptrElements) {
    unsigned char lengths[MAX_CODE];
    for (int i = 0; i < MAX_CODE; ++i) {
        int targetIndex = ptrElements[i].word;
        lengths[targetIndex] = ptrElements[i].codeLength;
    }
    fwrite(lengths, sizeof(unsigned char), MAX_CODE, outputFile);
    bitsNumber += (MAX_CODE * 8);
}


void writeCompressedFile(unsigned char *buffer, int bufferSize, FILE *outputFile, Element *ptrElements) {

    fseek(outputFile, 1, SEEK_SET); // lascio spazio per il byte di flag!
    writeLengths(outputFile, ptrElements);

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
    int lastBitsToWrite = 8 - (bitsNumber % 8);
    // completare l'ultimo byte con zeri
    for (int i = 0; i < lastBitsToWrite; ++i) {
        addBitToFile(0, outputFile);
        bitsNumber++;
    }
    // scrivo il byte di flag
    fseek(outputFile, 0, SEEK_SET);
    fputc(lastBitsToWrite, outputFile);
    bitsNumber += 8;
    fclose(outputFile);
}


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
    fread(&lastBits, sizeof(unsigned char), 1, inputFile);
    fread(lengths, sizeof(unsigned char), MAX_CODE, inputFile);

    for (int i = 0; i < MAX_CODE; ++i) {
        ptrElements[i].word = (unsigned char) i;
        ptrElements[i].codeLength = lengths[i];
    }
}


void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer) {
    fseek(inputFile, MAX_CODE + 1, SEEK_SET);
    fread(buffer, sizeof(unsigned char), bufferSize, inputFile);
}


void createDecodingTree(Node *root, const char *code, int index, unsigned char word) {
    if (code[index] != '\0') {
        if (code[index] == '0') {
            if (root->leftChild == NULL) {
                Node *lNode = createNode(NULL, NULL, -1, -1);
                root->leftChild = lNode;
                index++;
                createDecodingTree(root->leftChild, code, index, word);
            } else {
                index++;
                createDecodingTree(root->leftChild, code, index, word);
            }
        } else {
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
    } else {
        root->start = word;
        root->end = word;
        return;
    }
}


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


char *byteToChars(unsigned char byte) {
    char *chars = malloc(8 * sizeof(char));
    for (int i = 1; i <= 8; ++i) {
        chars[i - 1] = (char) extractBitOnPosition(byte, i);
    }
    return chars;
}


unsigned char *writeDecompBuffer(Node *root, unsigned char *bufferIn, int bufferSize) {

    Node *actualRoot = root;
    unsigned int initialBuffSize = 512;
    unsigned char *bufferOut = malloc(initialBuffSize * sizeof(unsigned char));

    unsigned int indexBuffIn = 0;
    unsigned int indexBuffOut = 0;

    while (indexBuffIn < bufferSize) {
        char *byte = byteToChars(bufferIn[indexBuffIn]);
        for (int i = 0; i < 8; ++i) {
            // trascurare gli ultimi bit che non fanno parte del file originale
            if (indexBuffIn == bufferSize - 1 && i == (8 - lastBits)) {
                break;
            }

            if (byte[i] == '0')
                root = root->leftChild;
            else
                root = root->rightChild;

            if (root->leftChild == NULL) {
                if (indexBuffOut == initialBuffSize - 1) {
                    initialBuffSize <<= 1;
                    bufferOut = realloc(bufferOut, initialBuffSize * sizeof(unsigned char));
                }
                bufferOut[indexBuffOut] = (unsigned char) root->start;
                indexBuffOut++;
                root = actualRoot;
            }
        }

        indexBuffIn++;
        free(byte);
    }
    return bufferOut;
}


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
    return decompBuffer;
}

