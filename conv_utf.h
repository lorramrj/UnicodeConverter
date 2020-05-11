#pragma once
#include <stdio.h>
#include <stdlib.h>

FILE* openFileTypeReadBinary(char* nameFile);
FILE* openFileTypeWriteBinary(char* nameFile);

int countBytes(unsigned char startByte);
unsigned char filterByteStart(unsigned char startByte);
unsigned char filterByteSequence(unsigned char sequenceByte);
int utf8_32(FILE* arq_entrada, FILE* arq_saida);

int setIntervalUNICODE(unsigned int caracter);
unsigned char getMaskStartByte(int sizeBytes);
int utf32_8(FILE* arq_entrada, FILE* arq_saida);
