/*
 ============================================================================
 Name        : basicCopy.c
 Author      : W. Schilling
 Version     :
 Copyright   : Your copyright notice
 Description : This program demonstrates how to make a basic copy of a file using file reads and writes.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

long copy(char* src, char* dest);

/**
 * This is the main function.  It starts the program.
 * @param argc This is the number of arguments.  For the program to work it must be 3.
 * @param argv These are the arguments.  arg[0] is the name of the program.  arg[1] is the source file.  arg[2] is the destination file.
 * @return
 */
int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: copy <src> <dest>\n");
		exit(-1);
	}
	// Perform the copy.
	int bytescopied = copy(argv[1], argv[2]);
	printf("Copy of %d bytes from %s to %s is now completed.\n", bytescopied,
			argv[1], argv[2]);
	return EXIT_SUCCESS;
}

/*
 * This file will implement a method to copy from one file to another file.
 */

/**
 * This function will copy from the source file into the destination file.
 * @param src This is the source filename.
 * @param dest This si the destination filename.
 */
long copy(char* src, char* dest) {
	// Open the source file for reading and the destination file for writing.
	FILE *sfp;
	FILE *dfp;
	long retVal;
	long count;

	retVal = 0;

	if (src != NULL && dest != NULL) {
		// Open the files for reading and writing.
		sfp = fopen(src, "rb");
		dfp = fopen(dest, "wb");

		count = 0;
		// Make sure that the source and destination files are valid.  If not, indicate an error with a -1 return.
		if (sfp == NULL || dfp == NULL) {
			retVal = -1;
		} else {
			char c;

			do {
				// Read in a byte from the source.
				c = fgetc(sfp);
				if (c != EOF) {
					// Write out the byte to the destination.
					fputc(c, dfp);
					count++;
				}
			} while (c != EOF);
			retVal = count;
		}
	}
	return retVal;
}
