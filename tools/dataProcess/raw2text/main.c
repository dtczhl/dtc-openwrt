/*
 * Converts raw bytes to texts
 *     For my debugfs outputs
 * 
 * Paramenters:
 *     -i (required)    inputFilePath
 *     -o (required)    outputFilePath
 * E.g., 
 *     raw2text -i inputFile -o outputFile
 *
 * Huanle Zhang
 * www.huanlezhang.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// some reasonable value
#define MAX_LENGTH 100 

FILE *fIn = NULL, *fOut = NULL;

void myExit(void);
void readBytes(void *, int);
void writeText(unsigned long);

void printFormat(void){
	printf("Format Error:\n");
	printf("	./main -i fileIn -o fileOut\n");
}

int main(int argc, char *argv[]){

	// format:
	//  ./main -i fileIn -o fileOut

	if (argc != 5){
		printFormat();
		return -1;
	}

	for (int i = 1; i < argc; i++){
	
		if (strcmp(argv[i], "-i") == 0){
			if (i < argc-1){
				i++;
				fIn = fopen(argv[i], "r");
				if (!fIn){
					printf("cannot open file: %s for read\n", argv[i]);
					return -1;
				}
			} else {
				printFormat();
				return -1;
			}
		} else if (strcmp(argv[i], "-o") == 0){
			if (i < argc-1){
				i++;
				fOut = fopen(argv[i], "w");
				if (!fOut){
					printf("cannot open file: %s for write\n", argv[i]);
					return -1;
				}
			} else{
				printFormat();
				return -1;
			}
		} else {
			printFormat();
			return -1;
		}
	}

	unsigned int length = 0;
	unsigned long ts1, ts2;
	unsigned char payload;

	while(1){
		readBytes(&length, 4);
		if (length > MAX_LENGTH) {
			myExit();
		}
		readBytes(&ts1, 8); 
		readBytes(&ts2, 8); 
		
		writeText(ts1); fprintf(fOut, ",");
		writeText(ts2); fprintf(fOut, ",");
		for (int i = 0; i < length; i++){
			readBytes(&payload, 1);
			writeText(payload);
			fprintf(fOut, "-");
		}
		fprintf(fOut, "\n");
	}

	myExit();
	return 0;
}

void myExit(void){
	fclose(fIn);
	fclose(fOut);
	exit(0);
}

void readBytes(void *pData, int byteLength){

	unsigned char tempBuf[16]; // 8 should be enough
	for (int i = 0; i < byteLength; i++){
		if (feof(fIn))
			myExit();
		fscanf(fIn, "%c", &tempBuf[i]);
	}
	if (byteLength == 1){
		// char
		*((unsigned char *)pData) = *((unsigned char *)tempBuf);
	} else if (byteLength == 4){
		// int
		*((unsigned int *)pData) = *((unsigned int*)tempBuf);
	} else if (byteLength == 8){
		// long
		*((unsigned long *)pData) = *((unsigned long*)tempBuf);
	} else {
		// error
		printf("length error\n");
	}
}

void writeText(unsigned long data){
	fprintf(fOut, "%ld", data);
}
