#include "common.h"


int get_directive(char *b, int type)
{
	if (DEBUG)
		printf("get_directive\n");
	
	int i, intv;
	int16_t v;
	char tok[10], strv[10], key[10];
	b+=4;
	if((i = get_token(tok, &b)) == -1)
		return -1;
	strncpy(key, tok, 10);

	memset(&tok, 0, 10);
	if((i = get_token(tok, &b)) == -1)
		return -1;
		
	if(strncmp(tok, "=", 1))
		return -1;

	memset(&tok, 0, 10);
	if((i = get_token(tok, &b)) == -1)
		return -1;

	if (isdigit(tok[0]) || tok[0] == '$' || tok[0] == '%')
	{
		if (get_val(tok, &v, VAL_8) == -1)
			return -1;
		intv = (int)v;
	} else {
		strncpy(strv, tok, 10);
		intv = 0;
	}

	if (type == EQU_T) {
		if (put_equate(key, strv, intv) == -1)
			return -1;
	} else
	if (type == DEF_T) {
		if (put_def(key, strv) == -1)
			return -1;
	}

	return 0;
}

int get_org(line *in, char *b)
{
	int16_t v;
	char tok[10];

	if (DEBUG)
		printf("get_org\n");
	
	b+=4;
	if (get_token(tok, &b) == -1)
		return -1;


	if (isdigit(tok[0]) || tok[0] == '$' || tok[0] == '%') {
		if (get_val(tok, &v, VAL_16) == -1)
			return -1;

		in->address = (uint16_t)v;
		in->size = 0;
		return 0;
	}
	return -1;
}

int put_equate(char *inkey, char *inval, int n)
{
	if (DEBUG) {
		printf("put_equate key:%s val%d\n", inkey, n);
	}
		

	if ((strncpy(equates[equate_count].key, inkey, 10)) == NULL)
		return -1;
	if (inval == NULL)
	{
		equates[equate_count].str_val[0] = '\0';
	} else {
		if ((strncpy(equates[equate_count].str_val, inval, 10)) == NULL)
			return -1;
	}
	equates[equate_count].int_val = n;
	equate_count += 1;
	return 0;
}

int get_equate(char *b, struct equate* eq)
{
	int i;

	if (equate_count <= 0)
		return -1;

	for (i = 0; i < equate_count; i++)
	{
		if (strncmp(b, equates[i].key, strlen(b)) == 0) {
			/* copy str_val or NULL if int val */
			if (!equates[i].str_val[0]) {
				eq->int_val = equates[i].int_val;
			} else {
				strncpy(eq->str_val, equates[i].str_val, 10);
			}

			return 0;
		}
	}
	fprintf(stderr, "equate not found!\n");
	eq = NULL;
	return -1;
}

int put_def(char *inkey, char *inval)
{
	if (DEBUG)
		printf("put_def\n");

	if ((strncpy(defs[def_count].key, inkey, 10)) == NULL)
		return -1;
	if (inval == NULL)
	{
		return -1;
	} else {
		if ((strncpy(defs[def_count].str_val, inval, 10)) == NULL)
			return -1;
	}
	def_count += 1;
	return 0;
}

int get_def(char *b, struct def* d)
{
	if (DEBUG)
		printf("get def\n");
	int i;

	if (def_count <= 0)
		return -1;

	for (i = 0; i < def_count; i++)
	{
		if (strncmp(b, defs[i].key, strlen(defs[i].key)) == 0) {
			/* copy str_val */
			strncpy(d->str_val, defs[i].str_val, 10);

			return 0;
		}
	}
	fprintf(stderr, "def not found!\n");
	d = NULL;
	return -1;
}

