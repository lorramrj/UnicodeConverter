#include <stdio.h>
#include <stdlib.h>
#include "conv_utf.h"

#define SIZE_FILE 200


FILE* openFileTypeReadBinary(char* nameFile)
{
	FILE* fp = fopen(nameFile, "rb");

	if (!fp)
	{
		printf("Error opening the file!\n\n");
		exit(1);
	}

	return fp;
}

FILE* openFileTypeWriteBinary(char* nameFile)
{
	FILE* fp = fopen(nameFile, "w+b");

	if (!fp)
	{
		printf("Error opening the file!\n\n");
		exit(1);
	}

	return fp;
}

/********************************************/
/***  PARTE CONVERSAO UTF-8 PARA UTF-32  ***/

int countBytes(unsigned char startByte) {

	int count = 0;
	while (startByte & 0x80)
	{
		count++;
		startByte = startByte << 1;
	}

	if (!count)
		return 1;
	return count;
}

unsigned char filterByteStart(unsigned char startByte)
{
	int count = countBytes(startByte);
	unsigned char mask = 0;
	unsigned char filter = 0;

	if (count == 1)
	{
		filter = startByte;
	}
	else if (count == 2)
	{
		mask = 0x1F;
		filter = startByte & mask;
	}
	else if (count == 3)
	{
		mask = 0x0F;
		filter = startByte & mask;
	}
	else if (count == 4)
	{
		mask = 0x07;
		filter = startByte & mask;
	}
	else
	{
		fprintf(stderr, "%s", "Erro filter!\n Erro na funcao ByteStart");
		exit(1);
	}

	return filter;
}

unsigned char filterByteSequence(unsigned char sequenceByte)
{
	unsigned char filter = 0;
	unsigned char mask = 0x3F;

	filter = sequenceByte & mask;
	return filter;
}

/*
unsigned int swapped(unsigned int num)
{
	unsigned ret  = ((num >> 24) & 0xff) | // move byte 3 to byte 0
		((num << 8) & 0xff0000) | // move byte 1 to byte 2
		((num >> 8) & 0xff00) | // move byte 2 to byte 1
		((num << 24) & 0xff000000); // byte 0 to byte 3

	return ret;

}*/

int utf8_32(FILE* arq_entrada, FILE* arq_saida)
{
	int count = 0;
	int idx = 0;
	int bitWise = 0;
	int desloc = 0;
	int increment = 1;

	unsigned int aux = 0;
	unsigned int final = 0;
	unsigned char pos = 0;
	unsigned char temp = 0;

	unsigned int caractere;
	unsigned char corr[4];

	unsigned int mount_utf32[SIZE_FILE];
	mount_utf32[0] = 0x0000FEFF; //BOM => vai ser gravado no arquivo em little endian devido meu processador

	while (!feof(arq_entrada))
	{
		caractere = fgetc(arq_entrada);
		if (caractere != EOF) {

			if (!count) //primeiro byte do caractere
			{
				count = countBytes(caractere);
				temp = filterByteStart(caractere);
				corr[idx] = temp;
			}
			else //sequencia de bytes do caractere, depois do primeiro
			{
				temp = filterByteSequence(caractere);
				corr[idx] = temp;
			}

			if (idx == (count - 1)) //se chegou no último byte do caractere
			{
				bitWise = 0;
				desloc = 2;

				while (idx >= 0)
				{

					if (!idx) //primeiro byte
					{
						pos = corr[idx];
					}
					else
					{
						pos = (corr[idx - 1] << (8 - desloc)) | corr[idx];
						corr[idx - 1] = corr[idx - 1] >> desloc;
					}

					aux = pos << bitWise;
					final = final | aux;



					bitWise += 8;
					aux = 0;
					desloc += 2;
					idx--;
				}

				mount_utf32[increment] = final;
				increment++;

				count = 0;
				final = 0;
				idx = 0;
			}
			else
			{
				idx++;
			}
		}
	}

	fwrite(mount_utf32, 4, increment, arq_saida);

	printf("Conversao de utf-8 para utf-32 realizada com sucesso!\n");

	fclose(arq_entrada);
	fclose(arq_saida);

}

/********************************************/
/***  PARTE CONVERSAO UTF-32 PARA UTF-8  ***/
int setIntervalUNICODE(unsigned int caracter)
{
	if (caracter <= 0x007F)         /* 1 byte */
		return 1;
	else if (caracter <= 0x07FF)    /* 2 bytes */
		return 2;
	else if (caracter <= 0xFFFF)    /* 3 bytes */
		return 3;
	else if (caracter <= 0x10FFFF)  /* 4 bytes */
		return 4;
}

unsigned char getMaskStartByte(int sizeBytes)
{
	if (sizeBytes == 1)
		return 0x00; //0000 0000

	else if (sizeBytes == 2)
		return 0xC0; //1100 0000

	else if (sizeBytes == 3)
		return 0xE0; //1110 0000

	else if (sizeBytes == 4)
		return 0xF0; //1111 0000
}

int utf32_8(FILE* arq_entrada, FILE* arq_saida)
{
	unsigned int caracter = 0;
	unsigned int aux = 0;
	unsigned int temp = 0;

	unsigned char mount = 0;
	unsigned char maskByteStart;
	unsigned char maskByteSequence = 0x80;

	int sizeBytes = 0;
	int count = 0;
	int idx = 0;

	int typeBOM; //0=> big-endian | 1=>little-endian

	unsigned char mount_utf8[SIZE_FILE];

	while (fread(&caracter, sizeof(int), 1, arq_entrada) == 1)
	{
		if (!idx) //primeiro byte => BOM
		{
			if (caracter == 0X0000FEFF) //big-endian
			{
				typeBOM = 0;
			}
			else if(caracter == 0XFFFE0000) //little-endian
			{
				typeBOM = 1;
			}
			else
			{
				fprintf(stderr, "%s", "Caractere BOM invalido!\n");
				exit(1);
			}
		}
		else
		{
			if (!typeBOM) //big-endian
			{
				aux = caracter;
			}
			else //little-endian
			{
				aux = ((caracter & 0x00FF0000) >> 8) | (caracter >> 24);
			}

			sizeBytes = setIntervalUNICODE(aux);
			maskByteStart = getMaskStartByte(sizeBytes);

			if (sizeBytes == 1)
			{
				temp = maskByteStart | aux;
				mount_utf8[count] = temp;
				count++;
			}
			else if (sizeBytes == 2)
			{
				temp = maskByteStart | (aux >> 6);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | (aux & 0x3F);
				mount_utf8[count] = temp;
				count++;
			}
			else if (sizeBytes == 3)
			{
				temp = maskByteStart | (aux >> 12);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | ((aux & 0xFC0) >> 6);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | (aux & 0x3F);
				mount_utf8[count] = temp;
				count++;
			}
			else if (sizeBytes == 4)
			{
				temp = maskByteStart | (aux >> 18);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | ((aux & 0x3F000) >> 12);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | ((aux & 0xFC0) >> 6);
				mount_utf8[count] = temp;
				count++;

				temp = maskByteSequence | (aux & 0x3F);
				mount_utf8[count] = temp;
				count++;
			}
		}

		idx++;
	}

	fwrite(mount_utf8, 1, count, arq_saida);

	printf("Conversao de utf-32 para utf-8 realizada com sucesso!\n");

	fclose(arq_entrada);
	fclose(arq_saida);
}

