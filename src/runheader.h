/* common headers for asm project */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h> // for usleep
#include <ncurses.h>
#include "instructions.h"
#define _GNU_SOURCE

#define DEBUG 0

#define DRD_MSK(x) (uint16_t)(x & 0xff00) /* direct dual */
#define BRCH_MSK(x) (uint16_t)(x & 0xfc00)
#define IMP_MSK(x) (uint16_t)(x & 0xffff)
#define IMM_MSK(x) (uint16_t)(x & 0xf000)
#define DRS_MSK(x) (uint16_t)(x & 0xff0f)
#define BIT_MSK(x) (uint16_t)(x & 0xff8f)
#define LSB_MSK(x) (uint16_t)(x & 0xff08)
#define LDD_MSK(x) (uint16_t)(x & 0xff0c)

#define C (uint8_t)0b00000001
#define Z (uint8_t)0b00000010
#define N (uint8_t)0b00000100
#define V (uint8_t)0b00001000
#define S (uint8_t)0b00010000
#define H (uint8_t)0b00100000
#define T (uint8_t)0b01000000
#define I (uint8_t)0b10000000

#define SCR_OFF 0xc000
#define SCR_MAXX 0x64
#define SCR_MAXY 0x28

/* for gpio */
#define IN 	0
#define OUT 	1
#define LOW	0
#define HIGH	1

typedef struct{
	uint16_t ip;
	uint16_t sp;
	uint8_t reg[16];
	uint8_t sreg;
}run_state;

typedef struct{
	WINDOW *screen;
	WINDOW *text;
	uint8_t	*old_win;
	int	gpio_rom;
	uint8_t *old_rom;
}_rom;

int sleep_v;

int load(char *, uint8_t **); 
int execute(run_state *, uint8_t **, _rom *);
int check_zsnf(uint8_t , run_state *);
int check_cv(uint16_t, uint8_t, uint8_t, run_state *);
int check_subf(uint16_t, uint8_t, uint8_t, run_state *);
int check_32(run_state *, uint8_t **);
uint8_t pop_val(run_state *, uint8_t **);
int push_val(uint8_t, run_state *, uint8_t **);
int inc_reg(uint8_t rg, run_state *);
int dec_reg(uint8_t rg, run_state *);
int run_io(uint16_t, run_state *, uint8_t **, _rom *);
int gpio_init();
int gpio_close();
int gpio_direction(int, int);
uint8_t gpio_read();
int gpio_write(int, int);
