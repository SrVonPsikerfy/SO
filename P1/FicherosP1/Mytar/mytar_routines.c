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
int copynFile(FILE * origin, FILE * destination, int nBytes)
{
	// number of bytes read
	int numBytes = 0;

	// while we haven't read all bytes, and we haven't reached the end 
	// of the file, we read from origin and write to destination
	int c; 
	while(numBytes < nBytes) {
		fread(&c, 1, 1, origin);

		if(feof(origin))
			break;
		
		fwrite(&c, 1, 1, destination);

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
char* loadstr(FILE * file) {
	
	// reads chars until the end of a string --> [\0] or 
	// the end of file [EOF]
	int c, numChars = 1; 
	while((c = getc(file)) != '\0'){
		if(c == EOF)
			return NULL;
		numChars++;
	}

	// return to the original start point in the stream
	fseek(file, -numChars, SEEK_CUR);

	// reserve space in heap and then store the string on the variable
	char* strVar = malloc(numChars);
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
stHeaderEntry* readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry* newHeader = NULL;

	// we read the number of files supposed to be on the header
	fread(&nFiles, sizeof(int), 1, tarFile);
	
	// memory reserve based on numFiles and the size of stHeaderEntry
	newHeader = malloc(sizeof(stHeaderEntry) * (*nFiles));

	// read the meta-information and stores it on the header
	// (name of the file, and size of the file)
	for (int i = 0; i < (*nFiles); i++) {
		newHeader[i].name = loadstr(tarFile);
		fread(&(newHeader[i].size), sizeof(int), 1, tarFile);
	}

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
int createTar(int nFiles, char *fileNames[], char tarName[]) {
	FILE *tarFile;
	//If there are not files to compress, we throw an error
	if(nFiles <= 0){
		fprintf(stderr, "%s", use);
		return EXIT_FAILURE;
	}

	//mTar failed in opening 
	if((tarFile = fopen (tarName, "w")) == NULL){
		fprintf(stderr, "The mtar file  %s can't be open.", tarName);
		perror(NULL);
		return EXIT_FAILURE;
	}

	stHeaderEntry *header;

	//Reserve nFiles * stHeaderEntry-MemorySize
	if((header = malloc(sizeof(stHeaderEntry) * nFiles)) == NULL ){
		perror("Can't allocate memory for the mtar Header");
		fclose(tarFile);
		remove(tarName);
		return EXIT_FAILURE;
	}

	unsigned int headerSize;
	//The minimun size the header could have is the size of an int (in case the nFiles = 0)
	headerSize = sizeof(int);
	
	//Calculation of max size of the header and save in the array each file name  
	for (int i = 0; i < nFiles; i++){
		int namesize = strlen(fileNames[i]) + 1;
		header[i].name = (char*) malloc(namesize); // NI ZORRA
		//In case we ran out of memory space, we throw an error
		if(header[i].name == NULL){
			perror("ERROR: Couldn't create tarBall");
			fclose(tarFile);
			remove(tarName);

			for (int j = 0; j < i; j++) // NI ZORRA
				free(header[j].name); 
			free(header); 

			return EXIT_FAILURE;
		}

		strcpy(header[i].name, fileNames[i]);
		headerSize += namesize + sizeof(header[i].size);
	}

	//We place the write pointer (headerSize) positions to write the content of each file
	fseek(tarFile, headerSize, SEEK_SET);

	FILE* inputFile;
	for(int i = 0; i < nFiles; i++) {		
		inputFile = fopen(fileNames[i], "r");
		if(inputFile == NULL){
			fprintf(stderr, "Can't open file %s . Check if the file name is correct. ",fileNames[i]);
			perror(NULL);
			fclose(tarFile);
			remove(tarName);

			for (int j = 0; j < i; j++)
				free(header[j].name);

			free(header); 

			return EXIT_FAILURE;
		}
		//Write the content of inputFile in tarFile. At the same time, calculates the size of the file, so we can save it
		header[i].size = copynFile(inputFile, tarFile, INT_MAX);

		//Cerramos el archivo abierto	
		fclose(inputFile);
	}

	//Rewind time. Restore the initial position of the writing pointer to write each FileName and its size
	rewind(tarFile);

	//Write nFiles at the beginning of the tarFile
	fwrite(&nFiles, sizeof(int), 1, tarFile);
	
	//Write the description of each file.
	for (int i = 0; i < nFiles; i++){
		fwrite(header[i].name, sizeof(char), strlen(header[i].name) + 1, tarFile);
		fwrite(&header[i].size, sizeof(header[i].size), 1 , tarFile);
	}
	fprintf(stdout,"The Mtar file was succesfully created!\n");

	for (int j = 0; j < nFiles; j++)
		free(header[j].name); //Ya que hemos reservado mememoria de esa string
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
	FILE *tarFile;

	if ((tarFile = fopen(tarName, "r")) == NULL) {
		fprintf(stderr, "The mtar file %s can't be open.", tarName);
		perror(NULL);
		return EXIT_FAILURE;
	}

	int nFiles;
	stHeaderEntry* header = readHeader(tarFile, &nFiles);

	for (int i = 0; i < nFiles; i++) {
		FILE* extractedFile;

		if ((extractedFile = fopen(header[i].name, "w")) == NULL) {
			fprintf(stderr, "The file %s couldn't be created.", header->name);
			perror(NULL);
			return EXIT_FAILURE;
		}

		copynFile(tarFile, extractedFile, header[i].size);

		fclose(extractedFile);
	}

	return EXIT_SUCCESS;
}