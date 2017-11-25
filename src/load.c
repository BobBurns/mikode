#include "runheader.h"

int load(char *fname, uint8_t **prog) 
{
	FILE *file;
	int n, i = 0;
	char header[8];

	if ((*prog = malloc(0xffff)) == NULL) {
		perror("malloc");
		return -1;
	}
	if ((file = fopen(fname, "r")) == NULL) {
		perror("fopen");
		return -1;
	}
	n = fread(header, 8, 1, file);
	if (!n) {
		perror("fread");
		exit(-1);
	}

	if (strncmp(header, "basm", 8)) {
		fprintf(stderr, "incorrect file format\naborted...");
		goto fatal;
	}

	while((n = fread(&(*prog)[i], 1, 1, file))) {
		if (++i > 0xffff) {
			fprintf(stderr, "file to large");
			goto fatal;
		}
	}
	if (DEBUG) 
		printf("%d bytes read\n", i);

	if (!feof(file)) {
		perror("file");
		fclose(file);
		goto fatal;
	}
	/* initialize screen rom */
	for (i = 0xc000; i < 0xd800; i++)
		(*prog)[i] = 0;

fatal:
	fclose(file);
	return 0;
}
