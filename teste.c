#include <stdio.h>
#include <stdlib.h>

FILE * openFileTypeReadBinary(char* nameFile)
{
	FILE* fp = fopen(nameFile, "rb");

	if (!fp)
	{
		printf("Error opening the file!\n\n");
		exit(1);
	}

	return fp;
}

long fileContentSize(FILE* fp)
{
	fseek(fp, 0L, SEEK_END);
	long count = ftell(fp);
	rewind(fp);  //Reinicializa a posição do ponteiro de arquivos para o início

	return count;
}

void* convert_utf8_to_utf32(FILE *fp)
{

}

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

int setBitOn(unsigned char startByte)
{
	int count = 0;
	while (!(startByte & 0x80))
	{
		count++;
		startByte = startByte << 1;
	}

	return count; //verificar o caso de 1byte
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
		printf("Error!!\n\n");
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

int utf8_32(FILE* arq_entrada, FILE* arq_saida)
{




}



int main(void)
{
	

	//long count = fileContentSize(fp)+10;
	//unsigned char *content = (unsigned char*) malloc((sizeof (char)) * (count));

	
	FILE* fp = openFileTypeReadBinary("utf8.txt");
	int count = 0;
	int idx = 0;
	int bitWise = 0;
	int setBitOnByteStart = 0;

	unsigned int aux = 0;
	unsigned int final = 0;

	unsigned int mount[100];
	int increment = 0;
	
	unsigned char pos = 0;
	unsigned char temp = 0;

	unsigned int caractere;
	unsigned char corr[4];

	while (!feof(fp))
	{
		caractere = fgetc(fp);
		if (caractere != EOF) {

			if (!count) 
			{
				count = countBytes(caractere);

				temp = filterByteStart(caractere);
				setBitOnByteStart = setBitOn(temp);

				corr[idx] = temp;
			}
			else
			{	
				temp = filterByteSequence(caractere);
				corr[idx] = temp;
			}

			if (idx == (count-1))
			{
				//logica do bitwise parece esta certa para little endien, mas ver porque não esta montando valores com mais de 1byte da maneira certa
				//na verdade não tenho que invertar a ordem dos bytes[
				//Alguns bytes ficaram diferentes!!

				idx = 0;
				bitWise = 32;
				while (idx < count)
				{   
					if (count > 1)
					{
						if (!idx)
						{
							bitWise -= (8-setBitOnByteStart);
						} 
						else 
						{
						   bitWise -= 6;
						}
					}
					else
					{
						bitWise -= 8;
					}

					pos = corr[idx];
					aux = pos << bitWise;
					final = final | aux;
					printf("-------\n");
					printf("%08X\n\n", pos);
					printf("%08X\n\n", aux);
					printf("%08X\n\n", final);
					printf("-------\n\n");
					aux = 0;

					idx++;
				}

				mount[increment] = final;
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

	int k = 0;
	while (k < increment)
	{
		printf("%08X\n", mount[k]);
		k++;
	}

	clearerr(fp);
	fclose(fp);



	return 0;
}