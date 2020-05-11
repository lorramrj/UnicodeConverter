#include <stdio.h>
#include <stdlib.h>
#include "conv_utf.h"



int main(void)
{
	FILE* arq_input_utf8 = openFileTypeReadBinary("utf8.txt"); 
	FILE* arq_output_utf32 = openFileTypeWriteBinary("utf32.txt");
	int sucess_convert_utf8_to_utf32 = utf8_32(arq_input_utf8, arq_output_utf32);


    //FILE* arq_input_utf32 = openFileTypeReadBinary("utf32.txt"); 
	//FILE* arq_output_utf8 = openFileTypeWriteBinary("utf8.txt");
	//int sucess_convert_utf32_to_utf8 = utf32_8(arq_input_utf32, arq_output_utf8);

	return 0;
}