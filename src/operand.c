#include "common.h"
#include "instructions.h"



/* return 0xff on error or register 0-15 on success */
uint8_t get_reg(char *b)
{
	//struct equate *eq = (struct equate *)malloc(sizeof(struct equate));;
	struct def df = {0};
	uint8_t d;
	char reg[3] = {'0','0','\0'};
	char *regp;
	int ret;
	if (b[0] != 'r')			
	{
		/* check for .equ */
		if ((ret = get_def(b, &df)) == -1) {
			fprintf(stderr, "error at get_equate\n");
			return FAIL8;
		}
		if (df.str_val == NULL || df.str_val[0] == '\0') {
			fprintf(stderr, "null str_val\n");
			return FAIL8;
		}
		/* TODO rewrite this to copy buffer */
		b = df.str_val;
	}
	if (!isdigit(reg[0] = b[1]))
		return FAIL8;
	if (!isdigit(reg[1] = b[2]))
	{
		reg[1] = '\0';
	}
	reg[3] = '\0';
	regp = strndup(reg, 3);
	d = (uint8_t)atoi(regp);
	free(regp);
	if (d > 15)
		return FAIL8;

	return (d & 0x0f);
}
int get_string(char **b, uint8_t ***dp, int index)
{
	if (DEBUG)
		printf("get string\n");
	char c;
	while((c = *(*b)) != '"')
	{
		if (c == '\0' || index > MAX_BUF)
			return -1;
		(*dp)[realloc_count -1][index] = (uint8_t)c;
		index++;(*b)++;
	}
	//(*b)++;
	return index;
}
		

/* return index to line buffer */
int get_operand(line * in, char **b, uint8_t ***dp)
{
	if (DEBUG)
		printf("get_operand\n");

	int err = 0, ret;
	int j = 0;
	uint8_t i = 0, d, dL, r, rL;
	//int16_t ival;
	int16_t v, bit;
	uint16_t kH, kL;
	char tok[10];
	uint8_t mask = 0;
	
	switch (in->type)
	{
	case DB_T:
		i += get_token(tok, b);
		while(j < MAX_DATA) {
			if (tok[0] == '\n')
				break;
			if (tok[0] == '"') {
				/* TODO handle string values */
				j = get_string(b, dp, j);
				if (j < 0) {
					err = ER_VAL;
					break;
				}
			} else 
			if (tok[0] == '\'') {
				(*dp)[realloc_count - 1][j] = (uint8_t)*(*b);
				(*b) += 2;j++;

			} else {
				ret = get_val(tok, &v, VAL_8);
				if (ret < 0 || v > 255) {
					err = ER_VAL;
					break;
				}
				(*dp)[realloc_count - 1][j] = (uint8_t)v;
				j++;
				(*b)--;
			}
			if (*(*b) != ',') {
				//(*b)++;
				break;
			}
			(*b)++;
			memset(tok, 0, 10);
			i += get_token(tok, b);
			
		}
		if (err > 0)
			break;
		
		realloc_count++;
		if ((realloc_count % (DATA_ALLOC )) == 0) {
			if (more_data(dp) == -1) {
				fprintf(stderr, "data realloc error.");
				return -1;
			}
		}
		in->dataidx = realloc_count - 1;
		in->size = j;
		in->opcode = 0xffff;
		break;

	case DIR_SING:
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		/* modify opcode */
		in->opcode = in->opcode | (d << 4) ;
		in->size = 2;
		break;
	case DIR_DUAL:	
		//i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		memset(tok, 0, 10);
		i += get_token(tok, b);
		
		if ((r = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		in->opcode = in->opcode | (d<<4) | r;
		in->size = 2;
		break;
	case DIR_STD:
		/* used to translate instruction eg clr Rd to eor Rd, Rr */
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		in->opcode = in->opcode | (d << 4) | d;
		in->size = 2;
		break;
	case IMM_BYTE:	
		i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}

		get_patch_label(tok, b);
		strncpy(in->patch_label, tok, MAX_LBL);
		in->patch = PATCH_IMM_BYTE;
		v = 0;

		kH = ((v << 4) & 0x0f00);
		kL = (v & 0x000f);
		in->opcode = in->opcode | (d<<4) | kH | kL;
		in->size = 2;
		break;
	case IMM_WORD:	
		i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ":", 1)) {
			err = ER_REG;
			break;
		}
		memset(tok, 0, 10);
		i += get_token(tok, b);
		
		if ((dL = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		if (d != dL+1 || dL < 8 || !(d % 2)) {
			err = ER_RAN;
			break;
		}
		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}

		get_patch_label(tok, b);
		strncpy(in->patch_label, tok, MAX_LBL);
		in->patch = PATCH_IMM_WORD;
		v = 0;


		/* 1001 0110 KKdd KKKK */
		kH = ((v << 2) & 0x00c0);
		kL = (v & 0x000f);
		in->opcode = in->opcode | ((dL & 0x06) << 3) | kH | kL;
		in->size = 2;
		break;
	case DIR_WORD:	
		/* Rd+1:Rd,Rr+1:Rr */
		i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ":", 1))
			return -1;
		memset(tok, 0, 10);
		i += get_token(tok, b);
		
		if ((dL = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		if (d != dL+1 || !(d % 2)) {
			err = ER_RAN;
			break;
		}
		memset(tok, 0, 10);
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}

		i += get_token(tok, b);
		if ((r = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ":", 1))
			return -1;
		memset(tok, 0, 10);
		i += get_token(tok, b);
		
		if ((rL = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		if (r != rL+1 || !(r % 2)) {
			err = ER_RAN;
			break;
		}

		rL = rL & 0x0f;
		dL = (dL & 0x0f) << 4;
		in->opcode = in->opcode | dL | rL;
		in->size = 2;
		break;
	case BRANCH:	
		/* handle branch to label */
		get_patch_label(tok, b);
		strncpy(in->patch_label, tok, MAX_LBL);
		in->patch = PATCH_BRANCH;
		v = 0;


		kH = (v & 0x007f) << 3;
		in->opcode = in->opcode | kH;
		in->size = 2;
		break;
	case BR_BIT:	
		/* bit value cannot be patched */
		i = 0;
		if ((ret = get_expr(in, &bit, b)) == -1) {
			fprintf(stderr ,"error get expr:%d\n", ret);
			err = ER_OPR;
			break;
		}
		/* inc pointer because it points at , after get_expr */
		(*b)++;
		/*
		if (get_val(tok, &bit, VAL_8) == -1) {
			err = ER_VAL;
			break;
		}
		*/
		if ((bit > 7) || (bit < 0)) {
			err = ER_VAL;
			break;
		}


		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		//(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}

		get_patch_label(tok, b);
		strncpy(in->patch_label, tok, MAX_LBL);
		in->patch = PATCH_BRANCH;
		v = 0;

		kH = (v & 0x007f) << 3;
		in->opcode = in->opcode | (bit & 0x07) | kH;
		in->size = 2;
		break;
	case BIT:	
		i = 0;
		if ((ret = get_expr(in, &bit, b)) == -1) {
			fprintf(stderr ,"error get expr:%d\n", ret);
			err = ER_OPR;
			break;
		}

		if ((bit > 7) || (bit < 0)) {
			err = ER_VAL;
			break;
		}
		in->opcode = in->opcode | ((bit & 0x07) << 4);
		in->size = 2;
		break;
	case LDSTR_BIT:	
		//i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		
		if ((ret = get_expr(in, &bit, b)) == -1) {
			fprintf(stderr ,"error get expr:%d\n", ret);
			err = ER_OPR;
			break;
		}

		if ((bit > 7) || (bit < 0)) {
			err = ER_VAL;
			break;
		}
		/* change this */
		in->opcode = in->opcode | ((d & 0x0f) << 4) | bit;
		in->size = 2;
		break;
	case IMPLIED:
		in->size = 2;
		break;

	case DIR_32:	
		i = 0;
		if (!isdigit(*(*b))) {
			get_patch_label(tok, b);
			strncpy(in->patch_label, tok, MAX_LBL);
			in->patch = PATCH_DIR32;
			v = 0;
		} else {
			i += get_token(tok, b);

			if (get_val(tok, &v, VAL_16) == -1) {
				err = ER_VAL;
				break;
			}
			if (v > 0xffff) {
				err = ER_VAL;
				break;
			}
		}
		in->opextra = v;
		in->size = 4;
		break;
	case LD_IND:	
		//i = 0;
		/* TODO rewrite for store eg Y,Rr */
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		memset(tok, 0, 10);
		i += get_token(tok, b);
		
		/* cmp x or y, x+ y+, -x -y */
		/* ld Rd,Y has 12bit set but I'm not going to bother with it */
		
		if (tok[0] == 'X'|| tok[0] == 'Y')
		{
		//	printf("c %c\n", *((*b)-1));
			if (tok[1] == '\0') {
				mask = 0b00;
			} else
			if (tok[1] == '+') {
				mask = 0b01;
			} else {
				err = ER_OPR;
				break;
			}
		} else
		if (tok[0] == '-' && (tok[1] == 'X' || tok[1] == 'Y')) {
				mask = 0b10;
		} else {
			err = ER_OPR;
			break;
		}

		in->opcode = in->opcode | (d << 4) | mask;
		in->size = 2;
		break;
	case STR_IND:	
		//i = 0;
		/* StoreIndY Y,rR */
		i += get_token(tok, b);
		
		if (tok[0] == 'X'|| tok[0] == 'Y')
		{
		//	printf("c %c\n", *((*b)-1));
			if (tok[1] == ',') {
				mask = 0b00;
			} else
			if (tok[1] == '+') {
				mask = 0b01;
			} else {
				err = ER_OPR;
				break;
			}
		} else
		if (tok[0] == '-' && (tok[1] == 'X' || tok[1] == 'Y')) {
				mask = 0b10;
		} else {
			err = ER_OPR;
			break;
		}

		memset(tok, 0, 10);
		/* dec pointer because it points after , */
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		memset(tok, 0, 10);
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}

		in->opcode = in->opcode | (d << 4) | mask;
		in->size = 2;
		break;
	case LD_DIR:	
		i = 0;
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		memset(tok, 0, 10);
		(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		/* start change to handle label + expression */
		if (!isdigit(*(*b))) {
			get_patch_label(tok, b);
			strncpy(in->patch_label, tok, MAX_LBL);
			in->patch = PATCH_DIR32;
			v = 0;
		} else {
			i += get_token(tok, b);

			if (get_val(tok, &v, VAL_16) == -1) {
				err = ER_VAL;
				break;
			}
			if (v > 0xffff) {
				err = ER_VAL;
				break;
			}
		}

		in->opcode = in->opcode | (d << 4);
		in->opextra = v;
		in->size = 4;
		break;
	case STR_DIR:	
		i = 0;
		if (!isdigit(*(*b))) {
			get_patch_label(tok, b);
			strncpy(in->patch_label, tok, MAX_LBL);
			in->patch = PATCH_DIR32;
			v = 0;
		} else {
			i += get_token(tok, b);

			if (get_val(tok, &v, VAL_16) == -1) {
				err = ER_VAL;
				break;
			}
			if (v > 0xffff) {
				err = ER_VAL;
				break;
			}
		}
		memset(tok, 0, 10);
		//(*b)--;
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		i += get_token(tok, b);
		if ((d = get_reg(tok)) == FAIL8) {
			err = ER_REG;
			break;
		}
		/*
		i += get_token(tok, b);

		if (strncmp(tok, ",", 1)) {
			err = ER_OPR;
			break;
		}
		if (!isdigit(*(*b))) {
			get_patch_label(tok, b);
			strncpy(in->patch_label, tok, MAX_LBL);
			in->patch = PATCH_DIR32;
			v = 0;
		} else {
			i += get_token(tok, b);

			if (get_val(tok, &v, VAL_16) == -1) {
				err = ER_VAL;
				break;
			}
			if (v > 0xffff) {
				err = ER_VAL;
				break;
			}
		}
		*/

		in->opcode = in->opcode | (d << 4);
		in->opextra = v;
		in->size = 4;
		break;
	default:
		err = ER_OPR;
	}

	/* handle errors */
	if (err < 0) {
		switch(err)
		{
		case ER_VAL:
			fprintf(stderr, "bad value\n");
			break;
		case ER_RAN:
			fprintf(stderr, "value out of range\n");
			break;
		case ER_OPR:
			fprintf(stderr, "bad operand.\n");
			break;
		}
		return -1;
	}

	return 0;
}




