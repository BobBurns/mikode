#include "common.h"


int syntax_error()
{
	fprintf(stderr, "systax error.\n");
	return 0;
}

int more_data(uint8_t ***dp)
{
	if (DEBUG)
		printf("more_data\n");

	int i;

	/* set should = 2 the first time this is called */
	int set = (realloc_count/(DATA_ALLOC)+1);
	*dp = realloc(*dp, set * DATA_ALLOC * sizeof(uint8_t *));
	if (*dp == NULL)
		return -1;

	/* allocate each block */
	for (i = 0 ; i < realloc_count + DATA_ALLOC; i++)
	{
		(*dp)[i] = realloc((*dp)[i],MAX_DATA * sizeof(uint8_t));
		if ((*dp)[i] == NULL) {
			fprintf(stderr, "null realloc");
			return -1;
		}
		//printf("allocated %d at index %d address %p\n", MAX_DATA, i, (*dp)[i]);
	}
	return 0;

}

int check_val_err(int val)
{
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) 
			|| (errno != 0 && val == 0))
	{
		perror("strtol");
		return -1;
	}
	return 0;
}


int get_func(char *b)
{
	printf("got func\n");
	return -1;
}

int get_val(char *b, int16_t *retval, int type)
{
	if (DEBUG)
		printf("get_val\n");
	int i, ival, base;
	char *endptr;
	errno = 0;

	if (isalpha(b[0]))
	{
		/* check if its a function */
		ival = get_func(b);
		/* maybe its an equate */
		if (ival < 0) {
			
			struct equate eq = {0};
			/* check for .equ */
			if (get_equate(b, &eq) == -1) {
				fprintf(stderr, "error at get_equate\n");
				return -1;
			}
			if (eq.str_val[0] != '\0') {
				fprintf(stderr, "not int_val \n");
				return -1;
			}
			*retval = (int16_t)eq.int_val;
			return 0;
		} else {
			*retval = ival;
			return 0;
		}
	}

	base = 10; i = 0;
	switch(b[0])
	{
	case '#':
		base = 10; i = 1;
		break;
	case '%':
		base = 2; i = 1;
		break;
	case '$':
		base = 16; i = 1;
		break;
	case '0':
		if (b[1] == 'x')
		{
			base = 16; i = 2;
			break;
		} else
		if (b[1] == 'b')
		{
			base = 2; i = 2;
			break;
		} else
		if (b[1] == 'o')
		{
			base = 8; i = 2;
			break;
		}
	}

	ival = strtol(&b[i], &endptr, base);
	/* check errors */
	if (check_val_err(ival) == -1)
		return -1;
	if (endptr == &b[i])
		return -1;

	switch (type) {
	case VAL_8:
		ival = (ival & 0x00ff);
		break;
	case VAL_16:
		ival = (ival & 0x0000ffff);
		break;
	}

	*retval = (int16_t)ival;
	return 0;
}


int errExit(char *b)
{
	/* temp */
	printf("error occured...\n%s\n", b);
	exit(-1);
}

