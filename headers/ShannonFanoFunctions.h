
/*
Shannon-Fano's Functions header file.
****************************************************************************
Author: Mohammad Halabi
Programming language: C standard version 99
****************************************************************************
*/

#ifndef LZS_SHF_SHANNONFANOFUNCTIONS_H
#define LZS_SHF_SHANNONFANOFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define MAX_CODE 256


/*************************** Structures ***********************************/

typedef struct element {
    unsigned char word;
    int frequency;
    char code[256];
    unsigned char codeLength;
} Element;

typedef struct node {
    int start, end;
    struct node *rightChild;
    struct node *leftChild;
} Node;

typedef struct code {
    char code[256];
} Code;

typedef struct codeBits {
    unsigned long long bits;
} CodeBits;


/********************** Compression's Functions Declaration ******************************/

void initializeElements(Element *ptrElements);

void calculateFrequencies(Element *ptrElements, unsigned char *buffer, int bufferSize);

void orderByFreqDesc(Element *ptrElements);

void orderBycodeLengthCresc(Element *ptrElements);

void orderByWord(Element *ptrElements);

long long int sumFrequencies(Element *ptrElements, Node *root);

int getSplitIndex(Element *ptrElements, Node *root);

Node *createNode(Node *lNode, Node *rNode, int start, int end);

void createEncodingTree(Element *ptrElements, Node *root);

void encode(Code *ptrCodes, Node *root);

void writeCodesForAllElements(Element *ptrElements, Code *ptrCodes);

char *fromNumToChars(long long num, int length);

void canonizeCodes(Element *ptrElements);

void writeByte(FILE *file);

void addBitToFile(unsigned char bit, FILE *file);

void writeLengths(FILE *outputFile, Element *ptrElements);

void writeCompressedFile(unsigned char *buffer, int bufferSize, FILE *outputFile, Element *ptrElements);

void compressSHF(char *toCompFileName, unsigned char *codedBuffer, int codedBufferCounter);


/********************** Decompression's Functions Declaration ******************************/


void readHeader(FILE *inputFile, Element *ptrElements);

void readCompressedFile(FILE *inputFile, unsigned int bufferSize, unsigned char *buffer);

void createDecodingTree(Node *root, const char *code, int index, unsigned char word);

void decode(Node *root, Element *ptrElements);

int extractBitOnPosition(unsigned char byte, int bitPosition);

char *byteToChars(unsigned char byte);

unsigned char *writeDecompBuffer(Node *root, unsigned char *bufferIn, int bufferSize);

unsigned char *decompressSHF(char *compFileName);


#endif //LZS_SHF_SHANNONFANOFUNCTIONS_H
