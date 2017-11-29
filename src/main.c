#include "common.h"
#include <getopt.h>

static int usage_flag;

int usage()
{
	printf("Usage: mikode <input file>\n");
	return 0;
}

int print_help()
{
	printf("********************************************************\n"
	       "*                                                      *\n");
	printf("* Mikode is a Assembly language compiler/emulator,     * \n"
	       "* built to learn basic assembly language constructs    *\n"
	       "* See README in the top level of the install directory *\n"
	       "* or http://github.com/bobburns/mikode for more        *\n"
	       "*                                                      *\n"
	       "********************************************************\n\n");
	usage();
	return 0;
}
int print_version()
{
	printf("\n** Mikode -- version 1.0 **\n");
	return 0;
}

int
main(int argc, char **argv)
{
	int c;
	int ret;
	char *filename;

	while (1)
	{
		static struct option long_options[] =
		{
			{"version", no_argument, &usage_flag, 1},
			{"help", no_argument, &usage_flag, 2},
			{ 0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "", long_options, &option_index);

		if (c == -1)
			break;

		switch(c)
		{
		case 0:
			if (usage_flag == 1) {
				print_version();
				exit(0);
			} else
			if (usage_flag == 2) {
				print_help();
				exit(0);
			}
			break;
		default:
			usage();
			exit(-1);
		}
	}

	if (optind >= argc) {
		usage();
		exit(-1);
	}

	filename = strndup(argv[optind], strlen(argv[optind]));

	ret = compile(filename);
	free(filename);
	if (ret < 0)
		exit(-1);

	exit(0);
}
