#include "runheader.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/* from https://elinux.org/RPi_GPIO_Code_Samples#Direct_register_access
 * Guillermo A. Amaral B. <g@maral.me>
 */


static int pin_map[8] = { 6, 13, 19, 26, 12, 16, 20, 21 };

int
gpio_init()
{
  char buffer[3];
  ssize_t bytes_written;
  int fd, i;

  /* open each indidividually */
  for (i = 0; i < 8; i++)
    {

      fd = open("/sys/class/gpio/export", O_WRONLY);
      if (fd == -1)
	{
	  fprintf(stderr, "Failed to open export for writing!\n");
	  return -1;
	}
      bytes_written = snprintf(buffer, 3, "%d", pin_map[i]);
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
gpio_close()
{
  char buffer[3];
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
      bytes_written = snprintf(buffer, 3, "%d", pin_map[i]);
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
   
#define DIRECTION_MAX 35
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
gpio_read(int pin)
{
#define VALUE_MAX 30
  char path[VALUE_MAX];
  char value_str[3];
  int fd, i;
  uint8_t result, val = 0;
	 
  for (i = 0; i < 8; i++)
    {

      snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin_map[pin]);
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
