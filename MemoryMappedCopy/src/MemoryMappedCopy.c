/*
 ============================================================================
 Name        : MemoryMappedCopy.c
 Author      : W. Schilling
 Version     :
 Copyright   : Your copyright notice
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

	// Check the usage provided by the user of the program.
    if (argc != 3)
	{
        fprintf(stderr, "%s source-file dest-file\n", argv[0]);
		exit(-1);
	}

	// Open the file that is the source as a read only file.
    fdSrc = open(argv[1], O_RDONLY);
    if (fdSrc == -1)
	{
        fprintf(stderr, "open failed.  Source file does not exist.\n");
		exit(-1);
	}
    /* Use fstat() to obtain size of file: we use this to specify the
       size of the two mappings */
    if (fstat(fdSrc, &sb) == -1)
	{
        fprintf(stderr, "fstat could not read statistics about the file.");
		exit(-1);
	}

    /* Handle zero-length file specially, since specifying a size of
       zero to mmap() will fail with the error EINVAL */
    if (sb.st_size == 0)
	{
		exit(EXIT_SUCCESS);
	}

	// Create a shared location in memory that is the source.
    src = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fdSrc, 0);
    if (src == MAP_FAILED)
	{
		fprintf(stderr, "mmap failed to create a shared partition");
		exit(-1);
	}

	// Open the destination file for writing.
    fdDst = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdDst == -1)
	{
		fprintf(stderr, "open of destination file failed.");
		exit(-1);
	}

    if (ftruncate(fdDst, sb.st_size) == -1)
	{
		fprintf(stderr, "ftruncate");
		exit(-1);
	}

	// Map the destination file to point to a mapped segment of memory.
    dst = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdDst, 0);
    if (dst == MAP_FAILED)
	{
		fprintf(stderr, "mmap");
		exit(-1);
	}

	// Copy the memory from the source to the destination.
    memcpy(dst, src, sb.st_size);       /* Copy bytes between mappings */

	if (msync(dst, sb.st_size, MS_SYNC) == -1)
	{
		fprintf(stderr, "msync");
		exit(-1);
	}

	// Close the files.
	close(fdSrc);
	close(fdDst);

	printf("Copy of %d bytes from %s to %s is now completed.\n", sb.st_size,
			argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
