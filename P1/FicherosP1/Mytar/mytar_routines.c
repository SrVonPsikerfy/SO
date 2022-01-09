#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int copynFile(FILE *origin, FILE *destination, int nBytes)
{
	// number of bytes read
	int numBytes = 0;

	// while we haven't read all bytes, and we haven't reached the end
	// of the file, we read from origin and write to destination
	int c;
	while (numBytes < nBytes)
	{
		if (fread(&c, 1, 1, origin))
			fwrite(&c, 1, 1, destination);

		if (feof(origin))
			break;

		numBytes++;
	}

	// return either numBytes read or -1 [nothing has been read]
	return (numBytes ? numBytes : -1);
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char *loadstr(FILE *file)
{

	// reads chars until the end of a string --> [\0] or
	// the end of file [EOF]
	int c, numChars = 1;
	while ((c = getc(file)) != '\0')
	{
		if (c == EOF)
			return NULL;
		numChars++;
	}

	// return to the original start point in the stream
	fseek(file, -numChars, SEEK_CUR);

	// reserve space in heap and then store the string on the variable
	char *strVar = malloc(numChars);
	fgets(strVar, numChars, file);

	// read \0 not read by fgets()
	fgetc(file);

	// returns string (may be NULL)
	return strVar;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function
 *  returns NULL.
 */
stHeaderEntry *readHeader(FILE *tarFile, int *nFiles)
{
	stHeaderEntry *newHeader = NULL;
	int numFiles;

	// we read the number of files supposed to be on the header
	fread(&numFiles, sizeof(int), 1, tarFile);

	// memory reserved based on numFiles and the size of stHeaderEntry
	newHeader = malloc(sizeof(stHeaderEntry) * numFiles); // ASK
	if (newHeader == NULL)
		return NULL;

	// read the meta-information and stores it on the header
	// (name of the file, and size of the file)
	for (int i = 0; i < numFiles; i++)
	{
		newHeader[i].name = loadstr(tarFile);
		fread(&(newHeader[i].size), sizeof(int), 1, tarFile);
	}

	(*nFiles) = numFiles;

	return newHeader;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int createTar(int nFiles, char *fileNames[], char tarName[])
{

	//If there are not files to compress, we throw an error
	if (nFiles <= 0)
	{
		fprintf(stderr, "%s", use);
		return EXIT_FAILURE;
	}

	FILE *tarFile = fopen(tarName, "w");

	//.mtar failed in opening
	if (tarFile == NULL)
	{
		fprintf(stderr, "The .mtar file %s can't be open.\n", tarName);
		return EXIT_FAILURE;
	}

	//Reserve nFiles * stHeaderEntry-MemorySize
	stHeaderEntry *header = malloc(sizeof(stHeaderEntry) * nFiles);
	if (header == NULL)
	{
		fprintf(stderr, "Can't allocate memory on the header array.\n");
		fclose(tarFile);
		return EXIT_FAILURE;
	}

	//The minimun size the header could have is the size of an int (in case the nFiles = 0)
	unsigned int headerSize = sizeof(int);

	//Calculation of max size of the header and save in the array each file name
	for (int i = 0; i < nFiles; i++)
	{
		int namesize = strlen(fileNames[i]) + 1;
		header[i].name = fileNames[i]; // ASK

		// header[i].name = (char*) malloc(namesize); // ASK

		// //In case we ran out of memory space, we throw an error
		// if (header[i].name == NULL) {
		// 	fprintf(stderr, "Unable to store file %s name.\n", fileNames[i]);
		// 	// perror("error"); // ASK

		// 	fclose(tarFile);
		// 	remove(tarName);

		// 	for (int j = 0; j < i; j++) // ASK
		// 		free(header[j].name);
		// 	free(header);

		// 	return EXIT_FAILURE;
		// }

		// strcpy(header[i].name, fileNames[i]);

		headerSize += namesize + sizeof(header[i].size);
	}

	//We place the write pointer (headerSize) positions to write the content of each file
	fseek(tarFile, headerSize, SEEK_SET);

	FILE *inputFile;
	for (int i = 0; i < nFiles; i++)
	{
		inputFile = fopen(fileNames[i], "r");
		if (inputFile == NULL)
		{
			fprintf(stderr, "Can't open file %s. Check if the name was written correctly.\n", fileNames[i]);

			// for (int j = 0; j < i; j++)
			// 	  free(header[j].name);
			free(header);

			fclose(tarFile);

			return EXIT_FAILURE;
		}

		//Write the content of inputFile in tarFile. At the same time, calculates the size of the file, so we can save it
		header[i].size = copynFile(inputFile, tarFile, INT_MAX);
		if (header[i].size == -1)
		{
			free(header);
			fclose(tarFile);
			fclose(inputFile);

			fprintf(stderr, "Unable to copy data from %s file.\n", fileNames[i]);
			return EXIT_FAILURE;
		}

		//close file
		fclose(inputFile);
	}

	//Rewind time. Restore the initial position of the writing pointer to write each FileName and its size
	rewind(tarFile);

	//Write nFiles at the beginning of the tarFile
	fwrite(&nFiles, sizeof(int), 1, tarFile);

	//Write the description of each file.
	for (int i = 0; i < nFiles; i++)
	{
		fwrite(header[i].name, sizeof(char), strlen(header[i].name) + 1, tarFile);
		fwrite(&header[i].size, sizeof(header[i].size), 1, tarFile);
	}
	fprintf(stdout, "The .mtar file was succesfully compressed!\n");

	// free up reserved space
	// for (int j = 0; j < nFiles; j++)
	// 	   free(header[j].name);
	free(header);

	fclose(tarFile);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int extractTar(char tarName[])
{
	FILE *tarFile = fopen(tarName, "r");
	// open the file, if it doesn't work, we stop and send an error message
	if (tarFile == NULL)
	{
		fprintf(stderr, "The .mtar file %s can't be open.\n", tarName);
		return EXIT_FAILURE;
	}

	// obtain the header block (array of names and sizes of each compressed file)
	int nFiles;
	stHeaderEntry *header = readHeader(tarFile, &nFiles);
	if (header == NULL)
	{
		fprintf(stderr, "Unable to allocate memory to store the header data.\n");
		fclose(tarFile);
		return EXIT_FAILURE;
	}

	// for each file compressed, we extract its data back into its original file
	for (int i = 0; i < nFiles; i++)
	{
		FILE *extractedFile = fopen(header[i].name, "w");

		// if for some reason the file isn't created properly, we stop and free the already
		// reserved and occupied space
		if (extractedFile == NULL)
		{
			fprintf(stderr, "The file %s couldn't be extracted correctly.\n", header->name);

			for (int j = 0; j < nFiles; j++)
				free(header[j].name);
			free(header);

			fclose(tarFile);

			return EXIT_FAILURE;
		}

		// copy the data stored to the extracted new file
		copynFile(tarFile, extractedFile, header[i].size);

		fclose(extractedFile);
	}

	// success message
	fprintf(stdout, "Files were succesfully extracted!\n");

	// free up the storage reserved for the names and the headers
	goto FREE;

	fclose(tarFile);

FREE:
	for (int j = 0; j < nFiles; j++)
		free(header[j].name);
	free(header);

	return EXIT_SUCCESS;
}