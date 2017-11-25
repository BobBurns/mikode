#include "common.h"

int
main(int argc, char **argv)
{
	int ret;
	
	/* TODO getopts for more */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
		exit(-1);
	}

	ret = compile(argv[1]);
	if (ret < 0)
		exit(-1);

	exit(0);
}
