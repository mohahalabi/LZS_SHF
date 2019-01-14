//
// Created by Eric on 02/01/2019.
//
#include "../headers/LZSFunctions.h"


/**
 * byte: E' l'appoggio che utilizzo per scrivere i singoli bit sul file, questo infatti viene riempito con otto bit e poi scritto.
 * currentBit: Variabile che indica quanti bit sono stati inseriti all'interno del byte.
 * byteCounter: Serve per contare i caratteri letti nel file
 */
unsigned char byte;
int currentBit;
int byteCounter;

int codedBufferCounter;
int codedBufferSize;
unsigned char *codedBuffer;

unsigned char *getCodedBuffer() {
    return codedBuffer;
}

int getCodedBufferSize() {
    return codedBufferCounter;
}

/**
 * Funzione chiamata quando si passa il parametro "-c", fa partire la compressione del file gestendo eventuali eccezioni che potrebbero verificarsi.
 */
void LZSCompression(FILE *inputFile, unsigned int fileSize) {

    unsigned char *buffer = (unsigned char *) malloc((fileSize) * sizeof(unsigned char));
    codedBuffer = (unsigned char *) malloc(sizeof(unsigned char));

    fread(buffer, (size_t) fileSize, 1, inputFile);

    encoding(buffer, fileSize);

    free(buffer);

    fclose(inputFile);
}


/**
 * Questa funzione si occupa di scorrere il buffer per cercare pattern di caratteri che si ripetono, di aggiornare la finestra scorrevole e di scrivere l'end marker.
 * @param buffer: array contenente i byte presenti nel file.
 * @param fileSize: numero di elementi all'interno del buffer.
 * @param outputFile: file su cui scrivere i dati compressi.
 */
void encoding(unsigned char *buffer, int fileSize) {
    SlidingWindow slidingWindow;
    slidingWindowIntialization(&slidingWindow);
    currentBit = 0;
    byteCounter = 0;
    codedBufferCounter = 0;
    codedBufferSize = 1;
    while (byteCounter < fileSize) {
        findMatch(buffer, fileSize, &slidingWindow);
        updateWindow(&slidingWindow);
    }
    writeEndMarker();
}

/**
 * Funzione utilizzata per l'inizializzazione della mia finestra, tail settato a -1 mi permette di gestire facilmente il primo carattere del buffer.
 * @param slidingWindow: è la mia finestra scorrevole.
 */
void slidingWindowIntialization(SlidingWindow *slidingWindow) {
    slidingWindow->head = 0;
    slidingWindow->tail = -1;
}

/**
 * Funzione utilizzata per l'inizializzazione del match.
 * @param match
 */
void matchInitialization(Match *match) {
    match->length = 0;
    match->offset = 0;
}

/**
 * Questa funzione si occupa di scrivere la sequenza di bit opporutna sul file in base al match trovato precedentemente.
 * Se la length è minore di 2 si scrive un bit con valore 0 che indica che non è stato trovato nessun patterne successivamente il carattere trovato.
 * Se la length è maggiore o uguale a 2 si scrive un bit con valore 1 che indica che è stato trovato un pattern di caratteri che si ripetono.
 *
 * @param length: lunghezza del match trovato.
 * @param offset: offset del match trovato.
 * @param outputFile: file su cui scrivere i bit.
 * @param buffer
 * @return
 */
int writeSequence(int length, int offset, unsigned char *buffer) {
    if (length < 2) {
        addBitToBuffer(0);
        writeCharacter(buffer[byteCounter]);
        return 1;
    } else {
        addBitToBuffer(1);
        writeOffset(offset);
        writeLength(length);
        return length;
    }
}

/**
 * In questa funzione vengono cercati pattern ripetuti di caratteri.
 * Quello che faccio è confrontare l'elemento alla posizione byteCounter del buffer con gli elementi alle posizioni precedenti, se dovessi trovare un riscontro allora incomincio
 * a confrontare i caratteri successivi a quella posizione con quelli successivi alla posizione byteCounter. Dopo essere tornato indietro tutte le volte avrò come risultato il
 * match con la length maggiore e potro quindi scriverlo sul file.
 *
 * @param buffer: array contenente i byte letti dal file.
 * @param outputFile: file su cui scrivere i dati compressi.
 * @param fileSize: numero di elementi nel buffer.
 * @param slidingWindow: finestra scorrevole.
 * @return
 */
void findMatch(unsigned char *buffer, int fileSize, SlidingWindow *slidingWindow) {
    Match match;
    matchInitialization(&match);
    int length, i = slidingWindow->tail;
    if (slidingWindow->tail == -1) {
        addBitToBuffer(0);
        writeCharacter(buffer[byteCounter]);
        ++byteCounter;
    }
    while ((i >= slidingWindow->head) && (match.length != WINDOW_SIZE)) {
        if (buffer[byteCounter] == buffer[i]) {
            length = searchMaxLength(i, buffer, fileSize);
            if (length > match.length) {
                match.offset = byteCounter - i;
                match.length = length;
            }
        }
        i--;
    }
    byteCounter += writeSequence(match.length, match.offset, buffer);
}

/**
 * Cerca la length del match confrontando elementi uguali contigui all'interno del buffer, cerca anche gli elementi nel "futuro" (quando la length è maggiore dell'offset)
 * @param tail: posizione corrente della coda
 * @param buffer
 * @param fileSize
 * @return
 */
int searchMaxLength(int tail, unsigned char *buffer, int fileSize) {
    int length = 0, currentOffset = byteCounter;
    while ((buffer[currentOffset] == buffer[tail]) && (tail < fileSize) && (length <= WINDOW_SIZE)) {
        currentOffset++;
        tail++;
        length++;
    }
    return length;
}

void updateWindow(SlidingWindow *slidingWindow) {
    slidingWindow->tail = byteCounter - 1;
    if (slidingWindow->tail - 2047 < 0)
        /**finestra non piena*/
        slidingWindow->head = 0;
    else
        /**finestra piena*/
        slidingWindow->head = slidingWindow->tail - 2047;
}

void writeEndMarker() {
    addBitToBuffer(1);
    addBitToBuffer(1);
    addBitToBuffer(0);
    addBitToBuffer(0);
    addBitToBuffer(0);
    addBitToBuffer(0);
    addBitToBuffer(0);
    addBitToBuffer(0);
    addBitToBuffer(0);
    while (currentBit != 0) {
        addBitToBuffer(0);
    }
}

/**
 * Questa funzione shifta i bit passati all'interno del byte e incrementa il contatore dei bit inseriti.
 * @param bit: bit passato.
 * @param file
 */


void addBitToBuffer(unsigned char bit) {
    currentBit++;
    byte = byte << 1 | bit;
    writeByteToBuffer();
}

/**
 * Quando il numero di bit shiftati all'interno del byte è pari a 8 posso scriverlo su file e riazzerare il contatore.
 * @param file
 */

void writeByteToBuffer() {
    if (currentBit == 8) {
        codedBuffer[codedBufferCounter] = byte;
        if (codedBufferCounter == codedBufferSize - 1) {
            codedBufferSize <<= 1;
            codedBuffer = realloc(codedBuffer, sizeof(unsigned char) * codedBufferSize);
        }
        ++codedBufferCounter;
        currentBit = 0;
        byte = 0;
    }
}


/**
 * Funzione per la scrittura di un carattere sul file.
 * @param character: carattere da scrivere.
 * @param outputFile
 */
void writeCharacter(unsigned char character) {
    for (int j = 7; j >= 0; j--) {
        (character & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
    }
}

/**
 * Questa funzione scrive un bit con valore 1 se l'offset passato come parametro ha un valore inferiore a 127 (ossia è possibile scriverlo con massimo 7 bit) e in seguito
 * scrive i bit dell'offset. Se invece l'offset passato è maggiore di 127 si scrive un bit con valore 0 e in seguito 11 bit rappresentanti l'offset.
 * @param offset
 * @param outputFile
 */
void writeOffset(int offset) {
    if (offset <= 127) {
        addBitToBuffer(1);
        for (int j = 6; j >= 0; j--) {
            (offset & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
        }
    } else {
        addBitToBuffer(0);
        for (int j = 10; j >= 0; j--) {
            (offset & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
        }
    }
}


/**
 * In base a quanto vale la lunghezza passata vengono scritti determinati bit (specificati nella tabella dello standard).
 * @param length: lunghezza del match.
 * @param outputFile
 */
void writeLength(int length) {
    if (length <= 4) {
        for (int j = 1; j >= 0; j--) {
            ((length - 2) & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
        }
        return;
    }
    if (length <= 7) {
        addBitToBuffer(1);
        addBitToBuffer(1);
        for (int j = 1; j >= 0; j--) {
            ((length - 5) & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
        }
        return;
    }
    if (length > 7) {
        int nbig = (length + 7) / 15;
        for (int i = 0; i < nbig; ++i) {
            addBitToBuffer(1);
            addBitToBuffer(1);
            addBitToBuffer(1);
            addBitToBuffer(1);
        }
        int x = length - ((nbig * 15) - 7);
        for (int j = 3; j >= 0; j--) {
            ((x) & (1 << j)) ? addBitToBuffer(1) : addBitToBuffer(0);
        }
        return;
    }
}

/**
 * funzione che restituisce la dimensione del file
 */
unsigned int getFileSize(FILE *inputFile) {
    fseek(inputFile, 0L, SEEK_END);
    unsigned int size = ftell(inputFile);
    rewind(inputFile);
    return size;
}


/**
 * byteFounded: byte trovato
 * endMarker: in questa variabile ci shifto dentro tutti i bit che leggo, quando sarà uguale a 110000000 il programma finisce
 * adress: è l'indice del circular buffer, mi serve per sapere dove inserire l'ultimo elemento
 * circular buffer: struttura che utilizzo per salvarmi i match trovati, ha una dimensione di 2048 elementi
 */
unsigned char byteFounded;
unsigned char endMarker;
int adress;
unsigned char circolarBuffer[WINDOW_SIZE];

void LZSDecompression(int argc, unsigned char *buffer, char *nameOutputFile) {
    FILE *outputFile = fopen(nameOutputFile, "wb");
    decoding(buffer, outputFile);
    free(buffer);
    fclose(outputFile);
}

/**
 * eseguo il seguente procedimento fino a quando non trovo l'end marker:
 * 1)leggo il bit di controllo, se questo è pari a 0 leggo altri 8 bit (situazione in cui la length del match è minore di 2), poi aggiungo al circular buffer il carattere lettp
 * 2)se il bit di controllo invece è pari a 1, leggo l'offset e la lunghezza del match e poi lo aggiungo al mio circular buffer (situazione in cui la length è maggiore di 2)
 *
 * @param buffer
 * @param outputFile
 */
void decoding(unsigned char *buffer, FILE *outputFile) {
    staticIniatializer(buffer);
    while (1) {
        unsigned char control = readBit();
        takeNextByte(buffer);
        if (control == 0) {
            byteFounded = 0;
            for (int j = 0; j < 8; ++j) {
                unsigned char bit = readBit();
                takeNextByte(buffer);
                byteFounded = byteFounded << 1 | bit;
            }
            fwrite(&byteFounded, sizeof(unsigned char), 1, outputFile);
            addBuffer(byteFounded);
        } else if (control == 1) {
            Match match;
            match.offset = offsetIdentification(buffer);
            if (match.offset == -1)
                return;
            match.length = lengthIdentification(buffer);
            addMatch(match.offset, match.length, outputFile);
        }
    }
}

/**
 * inizializzo tutte le variabili statiche che utilizzo all'interno del progetto
 * @param buffer
 */
void staticIniatializer(unsigned char *buffer) {
    adress = 0;
    byte = 0;
    currentBit = 0;
    byteCounter = 0;
    byte = buffer[0];
}

/**
 * 1) Leggo un bit di controllo, se questo bit è pari ad 1, vuol dire che l'offset è minore di 128 e possono quindi leggere i prossimi 7 bit e shifarli all'interno di
 *    offset, se invece vale 0 leggo i successivi 11 bit e li shifto dentro ad offset.
 * 2) poi ritorno offset
 * 3)se mi trovo nel primo caso dell'if vuol dire che ho già inserito due '1' all'interno della variabile endMarker, se a questo punto dovessi inserire 7 '0' allora la
 *   funzione ritornerebbe -1 e il programma finirebbe
 * @param buffer
 * @return
 */
int offsetIdentification(unsigned char *buffer) {
    unsigned char offsetBit = readBit();
    takeNextByte(buffer);
    int offset = 0;
    endMarker = 0;
    if (offsetBit == 1) {
        for (int j = 0; j < 7; ++j) {
            unsigned char bit = readBit();
            takeNextByte(buffer);
            endMarker = endMarker << 1 | bit;
            offset = offset << 1 | bit;
        }
        if (endMarker == 0) {
            return -1;
        }
    } else if (offsetBit == 0) {
        for (int j = 0; j < 11; ++j) {
            unsigned char bit = readBit();
            takeNextByte(buffer);
            offset = offset << 1 | bit;
        }
    }
    return offset;
}

/**
 * per prima cosa leggo i primi 2 bit e ne ritorno il valore corrispondente, se però i bit letti sono due '1' allora leggo altri due bit, e ritorno
 * il valore corrispondente specificato dall'algoritmo dei quattro bit che ho letto in totale, se pure questi due ultimi bit sono due '1' allora
 * siamo nella situazione in cui la length è maggiore di 7 e devo gestirla diversamente.
 * in questo caso devo leggere dei nibble, fino a quando questi saranno diversi da 1111 (15), ogni volta che leggo un nibble con valore 15 incremento la
 * variabile n di 1, infine ritorno il valore prodotto dalla seguente formula: (length + (n * 15) - 7).
 * @param buffer
 * @return
 */
int lengthIdentification(unsigned char *buffer) {
    int length = 0;
    for (int j = 0; j < 2; ++j) {
        unsigned char bit = readBit();
        takeNextByte(buffer);
        length = length << 1 | bit;
    }
    if (length != 3) {
        return length + 2;
    } else {
        for (int j = 0; j < 2; ++j) {
            unsigned char bit = readBit();
            takeNextByte(buffer);
            length = length << 1 | bit;
        }
        if (length == 15) {
            int n = 0;
            while (length == 15) {
                ++n;
                length = 0;
                for (int j = 0; j < 4; ++j) {
                    unsigned char bit = readBit();
                    takeNextByte(buffer);
                    length = length << 1 | bit;
                }
            }
            return (length + (n * 15) - 7);
        } else {
            return length - 7;
        }
    }
}

/**
 * per aggiungere un nuovo match al circular buffer, devo retrocedere rispetto all'indice del circular buffer di una quantità pari all'offset,
 * dopo inizio ad aggiungere all'ultima posizione del buffer gli elementi letti nella posizione in cui mi trovo e allo stesso momento li scrivo sul file
 * @param offset
 * @param length
 * @param ptr
 */
void addMatch(int offset, int length, FILE *ptr) {
    int matchAdress = adress - offset;
    if (adress < offset)
        matchAdress = WINDOW_SIZE + matchAdress;
    for (int i = 0; i < length; ++i) {
        addBuffer(circolarBuffer[matchAdress]);
        putc(circolarBuffer[matchAdress], ptr);
        matchAdress = (matchAdress + 1) % WINDOW_SIZE;
    }
}

/**
 * con questa funzione aggiungo un elemento al buffer e poi aggiorno l'indice d'inserimento, se mi trovassi nella posizione 2047 allora l'indirizzo
 * verrebbe aggiornato a 0, grazie al modulo riesco a dare una forma ciclica al mio buffer.
 * @param val
 */
void addBuffer(unsigned char val) {
    circolarBuffer[adress] = val;
    //incremento dell'indice del circular buffer
    adress = (adress + 1) % WINDOW_SIZE;
}

/**
 * questa funzione ha lo scopo di prendere il byte successivo che devo leggere dal file.
 *quando la variabile currentBit è pari a 8 vuol dire che ho letto 8 bit e quindi che alla variabile byte devo assegnarli il byte successivo da leggere
 * @param buffer
 */
void takeNextByte(unsigned char *buffer) {
    if (currentBit == 8) {
        currentBit = 0;
        byteCounter++;
        byte = buffer[byteCounter];
    }
}

/**
 * Con questa funzione combinata alla funzione takeNextByte posso leggere bit a bit dal file.
 * quello che faccio non è altro che ritornare il bit più significativo della variabile byte, ogni volta però faccio uno shift a sinistra,
 * cosi facendo leggo tutti i bit della variabile.
 * ogni volta che un bit viene letto currentBit viene incrementato di 1
 * @return
 */
unsigned char readBit() {
    unsigned char temp = byte;
    byte = byte << 1;
    currentBit++;
    return (temp & (1 << 7)) ? 1 : 0;
}