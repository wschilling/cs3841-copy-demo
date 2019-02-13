/*
 ============================================================================
 Name        : MemoryMappedCopy.c
 Author      : Michael Kerrisk, comments completed by W. Schilling
 Version     :
 Copyright   : This code is based upon / borrows from Copyright (C) 2019 Michael Kerrisk,
               though earlier versions can be found on the web.
 Description : This program will copy a file using memory mapped IO.
 ============================================================================
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */

/**
 * This is the main function.  It starts the program.
 * @param argc This is the number of arguments.  For the program to work it must be 3.
 * @param argv These are the arguments.  arg[0] is the name of the program.  arg[1] is the source file.  arg[2] is the destination file.
 * @return
 */
int main(int argc, char *argv[])
{
    char *src, *dst;
    int fdSrc, fdDst;
    struct stat sb;

	// Check the usage provided by the user of the program.  Must have 3 parameters.
    if (argc != 3)
	{
        printf("%s source-file dest-file\n", argv[0]);
		exit(-1);
	}

	// Open the file that is the source as a read only file.
    fdSrc = open(argv[1], O_RDONLY);
    if (fdSrc == -1)
	{
        fprintf(stderr, "open failed.  Source file does not exist.\n");
		exit(-1);
	}
	
    /* Use fstat() to obtain size of file: This will populate the buffer with file status information.
	   If the return is -1, there is an error.	*/
    if (fstat(fdSrc, &sb) == -1)
	{
        fprintf(stderr, "fstat could not read statistics about the file.");
		exit(-1);
	}

    /* A file that is of length 0 is a special file.  There is nothing to do as it is an empty file.	*/
    if (sb.st_size == 0)
	{
		exit(EXIT_SUCCESS);
	}

	// Create a shared location in memory that is the source.
	// The material is to be private segment.  The page is only to be readable.
    src = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fdSrc, 0);
    
	// Make sure that the operation was successful.
	if (src == MAP_FAILED)
	{
		fprintf(stderr, "mmap failed to create a shared partition");
		exit(-1);
	}

	// Open the destination file for writing.
	// The parameters are set accordingly.
    fdDst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdDst == -1)
	{
		fprintf(stderr, "open of destination file failed.");
		exit(-1);
	}

	// Limit the file to be exactly sb.st_size bytes in length.
    if (ftruncate(fdDst, sb.st_size) == -1)
	{
		fprintf(stderr, "ftruncate");
		exit(-1);
	}

	// Map the destination file to point to a mapped segment of memory.
	// By mapping it we can copy directly into the buffer.
    dst = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdDst, 0);
    if (dst == MAP_FAILED)
	{
		fprintf(stderr, "mmap");
		exit(-1);
	}

	// Copy the memory from the source to the destination.
	// A memcpy will copy from one buffer to the other exactly sb.st_size bytes.
    memcpy(dst, src, sb.st_size);       

	// The msync will synchronize the file on disc with the memory map.
	// The parameter MS_SYNC causes the task to block and wait until the update is completed.
	if (msync(dst, sb.st_size, MS_SYNC) == -1)
	{
		fprintf(stderr, "msync");
		exit(-1);
	}

	// Close the files.  This will write the results out to disk.
	close(fdSrc);
	close(fdDst);

	// Indicate to the user that the file copy is now completed.
	printf("Copy of %d bytes from %s to %s is now completed.\n", sb.st_size,
			argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
