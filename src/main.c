/* 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "common.h"
#include <getopt.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

static int usage_flag;

int
usage ()
{
  printf ("Usage: mikode [-rgs] <input file>\n");
  return 0;
}

int
print_help ()
{
  printf ("********************************************************\n"
	  "*                                                      *\n");
  printf ("* Mikode is a Assembly language compiler/emulator,     * \n"
	  "* built to learn basic assembly language constructs    *\n"
	  "* See README in the top level of the install directory *\n"
	  "* or http://github.com/bobburns/mikode for more        *\n"
	  "*                                                      *\n"
	  "********************************************************\n\n");
  usage ();
  return 0;
}

int
print_version ()
{
  printf ("\nMikode - %s\n", VERSION);
  printf ("Copyright (C) 2017  Bob Burns\n"
	  "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
	  "This is free software: you are free to change and redistribute it.\n"
	  "There is NO WARRANTY, to the extent permitted by law.\n");
  return 0;
}

int
main (int argc, char **argv)
{
  int c;
  int ret, run = 0;
  char *filename;
  int sleep_v = 1000;

  while (1)
    {
      static struct option long_options[] = {
	{"version", no_argument, &usage_flag, 1},
	{"help", no_argument, &usage_flag, 2},
	{"run", no_argument, 0, 'r'},
	{"gpio", no_argument, &usage_flag, 3},
	{"sleep", required_argument, 0, 's'},
	{0, 0, 0, 0}
      };
      int option_index = 0;
      c = getopt_long (argc, argv, "rgs:", long_options, &option_index);

      if (c == -1)
	break;

      switch (c)
	{
	case 0:
	  if (usage_flag == 1)
	    {
	      print_version ();
	      exit (0);
	    }
	  else if (usage_flag == 2)
	    {
	      print_help ();
	      exit (0);
	    }
	  break;
	case 'r':
	  run = 1;
	  break;
	case 'g':
	  usage_flag = 3;
	  break;
	case 's':
	  sleep_v = atoi(optarg);
	  break;
	default:
	  usage ();
	  exit (-1);
	}
    }

  if (optind >= argc)
    {
      usage ();
      exit (-1);
    }

  if (strlen (argv[optind]) > 255)
    {
      fprintf (stderr, "file name too long!\n");
      exit (-1);
    }

  filename = strndup (argv[optind], strlen (argv[optind]));

  if (run)
    ret = run_main (filename, usage_flag, sleep_v);
  else
    ret = compile (filename);

  free (filename);
  if (ret < 0)
    exit (-1);

  exit (0);
}
