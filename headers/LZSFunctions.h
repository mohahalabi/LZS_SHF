//
// Created by Eric on 02/01/2019.
//

#ifndef LZS_LZSFUNCTIONS_H
#define LZS_LZSFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define WINDOW_SIZE 2048
#define MIN_SIZE_TO_COMPRESS 260

typedef struct match {
    int length;
    int offset;
} Match;

typedef struct slidingWindow {
    int tail;
    int head;
} SlidingWindow;

/////////compressione//////////
void findMatch(unsigned char *buffer, int fileSize, SlidingWindow *slidingWindow);

int writeSequence(int length, int offset, unsigned char *buffer);

void updateWindow(SlidingWindow *slidingWindow);

void writeEndMarker();

void writeByteToBuffer();

void addBitToBuffer(unsigned char bit);

unsigned int getFileSize(FILE *inputFile);

void writeOffset(int offset);

void writeCharacter(unsigned char character);

void writeLength(int length);

void matchInitialization(Match *match);

void encoding(unsigned char *buffer, int fileSize);

void slidingWindowIntialization(SlidingWindow *slidingWindow);

int searchMaxLength(int tail, unsigned char *buffer, int fileSize);


void LZSCompression(FILE *inputFile, unsigned int fileSize);

unsigned char *getCodedBuffer();

int getCodedBufferSize();

////////decomopressione//////////
void LZSDecompression(int argc, unsigned char *buffer, char *nameOutputFile);

void decoding(unsigned char *buffer, FILE *outputFile);

unsigned char readBit();

void takeNextByte(unsigned char *buffer);

int lengthIdentification(unsigned char *buffer);

int offsetIdentification(unsigned char *buffer);

void addMatch(int offset, int length, FILE *ptr);

void addBuffer(unsigned char val);

void staticIniatializer(unsigned char *buffer);


#endif //LZS_LZSFUNCTIONS_H
