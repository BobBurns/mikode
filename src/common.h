/* common headers for asm project */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#define _GNU_SOURCE

#define DEBUG 0
#define MAX_BUF 512
#define MAX_DATA 255
#define MAX_LBL 100
#define MAX_TOK 20
#define DATA_ALLOC 20

#define REG_D 1
#define REG_R 2
#define VAL_8 3
#define VAL_16 4
#define EQU_T 1
#define DEF_T 2

#define IMM_ADDR 1
#define ACC_ADDR 2
#define ABS_ADDR 3
#define IND_ADDR 4
#define ZER_ADDR 5
#define ZER_IDX_ADDR 6
#define IDX_IND_ADDR 7
#define IND_IDX_ADDR 8

#define ER_VAL -1
#define ER_RAN -2
#define ER_REG -3
#define ER_OPR -4

#define FAIL8 0xff
#define NOCODE 0xffff
#define HOFF 8

#define PATCH_BRANCH 1
#define PATCH_DIR32 2
#define PATCH_VALL 3
#define PATCH_VALH 4
#define PATCH_IMM_BYTE 5
#define PATCH_IMM_WORD 6

typedef struct {
	char *text;
	char label[MAX_LBL];
	uint16_t size;
	int type;
	uint16_t opcode;
	uint16_t operand;
	uint16_t address;
//	uint8_t data[MAX_DATA];
	uint16_t opextra;
	int dataidx;
	uint8_t comment[MAX_DATA];
	uint8_t patch;
	char patch_label[MAX_LBL];
}line;

struct equate {
	char key[10];
	char str_val[10];
	int int_val;
};

struct def {
	char key[10];
	char str_val[10];
};

int get_operand(line *, char **, uint8_t ***);
int getopcode(line *, char *);
int get_token(char *, char **);
int get_comment(line *, char *);
int errExit(char *);
int put_equate(char *, char *, int );
int get_equate(char *, struct equate* );
int get_val(char *, int16_t *, int );
int get_directive(char *, int);
int more_data(uint8_t ***);
int get_func(char *);
int put_equate(char *, char *, int );
int put_def(char *, char *);
int get_equate(char *, struct equate* );
int get_def(char *, struct def* );
int errExit(char *);
int syntax_error();
int write_listing(line **, uint8_t ***, int, char *);
int write_asm(line **, uint8_t ***, int , char *);
int compile(char *);
int get_expr(line *, int16_t *, char **);
int get_org(line *, char *);
int get_patch_label(char *, char **);
int run_main (char*, int);
int getopcode(line *, char *);

int equate_count;
int def_count;
int realloc_count;
struct equate equates[255];
struct def defs[32];
