#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int c;

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);

	/* Read file byte by byte */
	while ((fread(&c, atoi(argv[2]), 1, file)) != 0) {
		/* Print byte to stdout */
		fwrite(&c, 1, 1, stdout);
	}

	fclose(file);
	return 0;
}
