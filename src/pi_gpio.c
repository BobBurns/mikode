/* get_chip_pins() adapted from 
 *
 * https://github.com/Groguard/CHIP_IO_C/blob/master/source/common.c
 *
 * This file incorporates work covered by the following copyright and
 * permission notice, all modified code adopts the original license:
 *
 * Copyright (c) 2013 Ben Croston
 * Copyright (c) 2016 Robert Wolterman
 * Copyright (c) 2013 Adafruit
 *
*/
#include "runheader.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_BUFFER 4096
#define CHIP_GPIO_PATH "/sys/class/gpio" 
#define CHIP_EXPANDER "pcf8574a\n"
#define VALUE_MAX 35
#define DIRECTION_MAX 40

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* from https://elinux.org/RPi_GPIO_Code_Samples#Direct_register_access
 * Guillermo A. Amaral B. <g@maral.me>
 */


static int pin_map[8];
int gpio_close();
int get_chip_pins(int*);
int get_pi_pins(int*);

int
gpio_init()
{
  char buffer[5];
  ssize_t bytes_written;
  int fd, i, err;
  int *p = pin_map;

  /* handle gpio mapping on chip or pi */
  if (HAVE__NTC_MODEL == 1)
    {
      err = get_chip_pins(p);
      if (err < 0)
	{
          fprintf(stderr, "error: get_chip_pins()\n");
          return err;
        } 
    }
  else if (HAVE__BCM_HOST == 1 && HAVE__DT_LNK)
    { 
      get_pi_pins(p);
    }
  else
/* will only happen if no config.h */
    {  
      printf("Sorry, cannot run with gpio. Could be there isn't a config.h"
	     " file or no directory types in dirent.h\n");
      return -1;
    }

  /* TODO make sure gpio is unexported first */

  /* open each indidividually */
  for (i = 0; i < 8; i++)
    {

      fd = open("/sys/class/gpio/export", O_WRONLY);
      if (fd == -1)
	{
	  fprintf(stderr, "Failed to open export for writing!\n");
	  return -1;
	}
      /* changing buffer to 5 to handle CHIP gpio */
      bytes_written = snprintf(buffer, 5, "%d", pin_map[i]);
      if ((write(fd, buffer, bytes_written)) == -1) 
	{
	  close(fd);
	  fprintf(stderr, "write to /sys/class/gpio/export failed\n");
	  return -1;
	}
	
      close(fd);
    }
    return 0;
}


int
gpio_close()
{
  char buffer[5];
  ssize_t bytes_written;
  int fd, i;

  /* close each indidividually */
  for (i = 0; i < 8; i++)
    {

      fd = open("/sys/class/gpio/unexport", O_WRONLY);
      if (fd == -1)
	{
	  fprintf(stderr, "Failed to open export for writing!\n");
	  return -1;
	}
      bytes_written = snprintf(buffer, 5, "%d", pin_map[i]);
      if ((write(fd, buffer, bytes_written)) == -1)
	{
	  close(fd);
	  return -1;
	}
      close(fd);
    }
    return 0;
}

int
gpio_direction(int pin, int dir)
{
  static const char s_directions_str[]  = "in\0out";
   
  char path[DIRECTION_MAX];
  int fd;
       
  snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin_map[pin]);
  fd = open(path, O_WRONLY);
  if (-1 == fd) 
    {
      fprintf(stderr, "Failed to open gpio direction for writing!\n");
      return(-1);
    }
	     
  if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3))
    {
      fprintf(stderr, "Failed to set direction!\n");
      return(-1);
    }
	       
  close(fd);
  return(0);
}
 
uint8_t
gpio_read()
{
  char path[VALUE_MAX];
  char value_str[3];
  int fd, i;
  uint8_t result, val = 0;
	 
  for (i = 0; i < 8; i++)
    {

      snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin_map[i]);
      fd = open(path, O_RDONLY);
      if (-1 == fd)
	{
	  fprintf(stderr, "Failed to open gpio value for reading!\n");
	  return(-1);
	}
	       
	if (-1 == read(fd, value_str, 3))
	  {
	    fprintf(stderr, "Failed to read value!\n");
	    return(-1);
	  }
		 
	  close(fd);
		   
	  result = (uint8_t)atoi(value_str);
	  val |= (result << i);
    }
  return val;
}
 
int
gpio_write(int pin, int value)
{
  static const char s_values_str[] = "01";
     
  char path[VALUE_MAX];
  int fd;
	 
  snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin_map[pin]);
  fd = open(path, O_WRONLY);
  if (-1 == fd) 
    {
      fprintf(stderr, "Failed to open gpio value for writing!\n");
      return(-1);
    }
	       
    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) 
      {
	fprintf(stderr, "Failed to write value!\n");
	return(-1);
      }
		 
    close(fd);
    return(0);
}

int 
get_chip_pins(int *pins)
{
  int base = 0, i;
  char label_file[MAX_BUFFER] = {0};
  FILE *label_fp;
  char base_file[MAX_BUFFER] = {0};
  FILE *base_fp;
  DIR *dir;
  struct dirent *ent;
  struct stat sbuf;

  if ((dir = opendir (CHIP_GPIO_PATH)) == NULL)
    return -2;

  /* iterante through /sys/class/gpio directories
   * looking for expander value to find base number
   * see
   * https://docs.getchip.com/chip.html#kernel-4-3-vs-4-4-gpio-how-to-tell-the-difference
   */
  /* TODO add rule in autoconf to test DT_SLNK */
  while ((ent = readdir (dir)) != NULL)
    {
      lstat (ent->d_name, &sbuf);
        /* if (S_ISDIR (sbuf.st_mode)) */
	/* I'm having issues with S_ISDIR returning true for file */
      if (ent->d_type == DT_LNK)
	{
	  if (!strcmp (".", ent->d_name) || !strcmp ("..", ent->d_name))
	    continue;

	  snprintf(label_file, sizeof (label_file), "%s/%s/label", CHIP_GPIO_PATH, 
		   ent->d_name);

	  if ((label_fp = fopen (label_file, "r")) == NULL)
	    {
	      perror ("cannot open CHIP label file!\n");
	      closedir (dir);
	      return -3;
	    }
	  char input_line[80] = {0};
	  char *s = fgets (input_line, sizeof (input_line), label_fp);
	  fclose (label_fp);

	  if (!strncmp (input_line, CHIP_EXPANDER, strlen (CHIP_EXPANDER)))
	    {
	      snprintf (base_file, sizeof (base_file), "%s/%s/base", CHIP_GPIO_PATH,
			ent->d_name);

	      if ((base_fp = fopen (base_file, "r")) == NULL)
		{
		  perror ("cannot open CHIP base file!\n");
		  closedir (dir);
		  return -3;
		}
	      s = fgets (input_line, sizeof (input_line), base_fp);
	      fclose (base_fp);

	      if (s == NULL)
		{
		  fprintf(stderr, "no CHIP base found!\n");
		  closedir (dir);
		  return -3;
		}
	      /* check for empty value */
	      char *endptr;
	      base = (int)strtol (input_line, &endptr, 10);
	      if (endptr == input_line)
		{
		  fprintf(stderr, "no CHIP base value. aborting...\n");
		  closedir (dir);
		  return -4;
		}
	      /* done */
	      break;
	      
	    }
	}
    }

  for (i = 0; i < 8; i++)
    {
      pins[i] = i + base;
    }
  return 0;
}
      
int
get_pi_pins(int *pins)
{
  *pins++ = 6; *pins++ = 13; *pins++ = 19; *pins++ = 26;
  *pins++ = 12, *pins++ = 16, *pins++ = 20; *pins++ = 21;
  return 0;
}
