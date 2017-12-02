#include "runheader.h"

#if DEBUG
#include <time.h>
#endif


int adc(uint16_t op, run_state *state)
{
	int r,d;
	uint16_t c, result;
	c = state->sreg & C;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] + state->reg[r] + c;

	check_cv(result, state->reg[d], state->reg[r], state);
	state->reg[d] = (uint8_t)result;
	check_zsnf(state->reg[d], state);
	//printf("adc result: %02x %02x\n", state->reg[d], state->sreg);
	state->ip += 2;
	return 0;
}


int add(uint16_t op, run_state *state)
{
	uint16_t result, r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	//printf("reg d: %x reg r: %x\n", state->reg[d], state->reg[r]);
	result = state->reg[d] + state->reg[r];
	check_cv(result, state->reg[d], state->reg[r], state);
	state->reg[d] = (uint8_t)result;
	check_zsnf(state->reg[d], state);
	//printf("add result: %02x %02x\n", state->reg[d], state->sreg);
	state->ip += 2;
	return 0;
}


int adiw(uint16_t op, run_state *state)
{
	uint8_t rg = 0x08, m = 0, c = 0;
	uint16_t result;
	uint16_t val;
	m = (uint8_t)(op >> 3) & 0x06;
	rg |= m;
	val = (op & 0x000f) | ((op & 0x00c0) >> 2);
	result = state->reg[rg] + val;
	if (result & 0x0100)
		c = 1;
	state->reg[rg] = (uint8_t)result;
	result = state->reg[rg+1] + c;
	check_cv(result, state->reg[rg+1], c, state);
	state->reg[rg+1] = (uint8_t)result;
	check_zsnf(state->reg[rg+1], state);
	/* check low byte for zero */
	if (state->reg[rg] && (state->sreg & Z))
		state->sreg &= ~Z;
	//printf("add result: %02x %02x %02x\n", state->reg[rg+1], state->reg[rg], state->sreg);
	state->ip += 2;
	return 0;
}

int and(uint16_t op, run_state *state)
{
	uint16_t r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	state->reg[d] = state->reg[d] & state->reg[r];
	/* v is cleared in this op */
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	//printf("and result: %02x %02x\n", state->reg[d], state->sreg);
	state->ip += 2;
	return 0;
}

int andi(uint16_t op, run_state *state)
{
	uint16_t d = (op & 0x00f0) >> 4;
	state->reg[d] &= (op & 0x000f) | ((op & 0x0f00) >> 4);
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	//printf("andi result: %02x %02x\n", state->reg[d], state->sreg);
	state->ip += 2;
	return 0;
}

int asr(uint16_t op, run_state *state)
{
	uint16_t result, d;
	uint8_t c, s;
	d = (op & 0x00f0) >> 4;
	c = (state->reg[d] & 0x01);
	s = (state->reg[d] & 0x80);
	result = (state->reg[d] >> 1) | s;
	state->reg[d] = (uint8_t)result;
	if (c)
		state->sreg |= c;
	else
		state->sreg &= ~C;
	/* V = N^C */
	check_zsnf(state->reg[d], state);
	state->sreg |= ((state->sreg ^ (state->sreg << 2)) & 4) << 1;
	//printf("asr result: %02x %02x\n", state->reg[d], state->sreg);
	state->ip += 2;
	return 0;
}
int bclr(uint16_t op, run_state *state)
{
	uint8_t b = (op & 0x0070) >> 4;
	state->sreg &= ~(1 << b);
	//printf("bclr result %02x\n", state->sreg);
	state->ip += 2;
	return 0;
}
int bld(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	uint8_t b = (op & 0x0007);
	uint8_t t = (state->sreg & 0x40) >> 6;
	if (t)
		state->reg[d] |= (1 << b);
	else
		state->reg[d] &= ~(1 << b);
	//printf("bld %02x %x %x\n", state->reg[d], b, t);
	state->ip += 2;
	return 0;
}
int branchs(uint16_t op, run_state *state)
{
	uint8_t b = op & 0x0007;
	uint8_t k = (op & 0x03f8) >> 3;
	//printf("k2 %x\n", k);
	if ((1 << b) & state->sreg) {
		if (k & 0x40) {
			state->ip -= (uint8_t)~(k | 0xc0) + 1;
			//printf("k3 %x\n", ~(k | 0xc0) + 1);
		}
		else 
			state->ip += k;
	} else {
		state->ip += 2;
	}
	return 0;
}
int branchc(uint16_t op, run_state *state)
{
	uint8_t b = op & 0x0007;
	uint8_t k = (op & 0x03f8) >> 3;
	//printf("k %x\n", k);
	if (!((1 << b) & state->sreg)) {
		if (k & 0x40)
			state->ip -= (uint8_t)~(k | 0xc0) + 1;
		else 
			state->ip += k;
	} else {
		state->ip += 2;
	}
	return 0;
}
int abrk(uint16_t op, run_state *state)
{
	state->ip += 2;
	return 1;
}
int bset(uint16_t op, run_state *state)
{
	uint8_t b = (op & 0x0070) >> 4;
	state->sreg |= (1 << b);
	//printf("bclr result %02x\n", state->sreg);
	state->ip += 2;
	return 0;
}
int bst(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	uint8_t b = (op & 0x0007);
	if (state->reg[d] & (1 << b))
		state->sreg |= T;
	else
		state->sreg &= ~T;
	state->ip += 2;
	return 0;
}
int call(uint16_t op, uint16_t op2, run_state *state, uint8_t **prog)
{
	state->ip += 4; /* return address */
	uint8_t l = (uint8_t)(state->ip & 0x00ff);
	uint8_t h = (state->ip & 0xff00) >> 8;
	push_val(l, state, prog);
	push_val(h, state, prog);
	state->ip = op2;
	return 0;
}
int clrf(uint16_t op, run_state *state)
{
	/* just debuging */
	/* clear all flags */
	state->sreg = 0;
	state->ip += 2;
	return 0;
}
int cbr(uint16_t op, run_state *state)
{
	uint16_t k = (~op) & 0x0f0f;
	op = (op & 0xf0f0) | k;
	andi(op, state);
	return 0;
}
int com(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	state->reg[d] = ~state->reg[d];
	state->sreg |= C;
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int cp(uint16_t op, run_state *state)
{
	int r,d;
	uint16_t result;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] - state->reg[r];

	check_cv(result, state->reg[d], state->reg[r], state);
	check_zsnf((uint8_t)result, state);
	state->ip += 2;
	return 0;
}
int cpc(uint16_t op, run_state *state)
{
	int r,d;
	uint16_t c, result;
	c = state->sreg & C;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] - state->reg[r] - c;

	check_cv(result, state->reg[d], state->reg[r], state);
	check_zsnf((uint8_t)result, state);
	state->ip += 2;
	return 0;
}
int cpi(uint16_t op, run_state *state)
{
	int d;
	uint16_t k, result;
	d = (op & 0x00f0) >> 4;
	k = (op & 0x000f) | ((op & 0x0f00) >> 4);
	result = state->reg[d] - k;
	check_cv(result, state->reg[d], k, state);
	check_zsnf((uint8_t)result, state);
	state->ip += 2;
	return 0;
}
int cpse(uint16_t op, run_state *state, uint8_t **prog)
{
	cp(op, state);
	if (!(state->sreg & Z))
		return 0;
	if (check_32(state, prog))
		state->ip += 2;
	state->ip += 2;
	return 0;
}
int dec(uint16_t op, run_state *state)
{
	int d;
	d = (op & 0x00f0) >> 4;
	if (state->reg[d] == 0x80)
		state->sreg |= V;
	else 
		state->sreg &= ~V;

	state->reg[d]-- ;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int eor(uint16_t op, run_state *state)
{
	int d, r;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	state->reg[d] ^= state->reg[r];
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int icall(uint16_t op, run_state *state, uint8_t **prog)
{
	/* using r15:r14 for call address */
	state->ip += 2; /* return address */
	uint8_t l = (uint8_t)(state->ip & 0x00ff);
	uint8_t h = (state->ip & 0xff00) >> 8;
	push_val(l, state, prog);
	push_val(h, state, prog);
	uint16_t addrh = (state->reg[15] << 8);
	state->ip = addrh | state->reg[14];
	return 0;
}
int ijmp(uint16_t op, run_state *state, uint8_t **prog)
{
	uint16_t addrh = (state->reg[15] << 8);
	state->ip = addrh | state->reg[14];
	return 0;
}
int inc(uint16_t op, run_state *state)
{
	int d;
	d = (op & 0x00f0) >> 4;
	/* see spec */
	if (state->reg[d] == 0x7f)
		state->sreg |= V;
	else 
		state->sreg &= ~V;

	state->reg[d]++ ;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int jmp(uint16_t op, uint16_t op2, run_state *state)
{
	state->ip = op2;
	return 0;
}
int lddx(uint16_t op, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	uint16_t iH = state->reg[13] << 8;
	uint16_t index = iH | state->reg[12];
	switch(op & 0x0003) {
	case 0x0000:
		/* no change to X */
		state->reg[rg] = (*prog)[index];
		break;
	case 0x0001:
		/* inc X after */
		state->reg[rg] = (*prog)[index];
		inc_reg(12, state);
		break;
	case 0x0002:
		dec_reg(12, state);
		state->reg[rg] = (*prog)[--index];
		break;
	}

	state->ip += 2;
	return 0;
}
int lddy(uint16_t op, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	uint16_t iH = state->reg[15] << 8;
	uint16_t index = iH | state->reg[14];
	switch(op & 0x0003) {
	case 0x0000:
		/* no change to X */
		state->reg[rg] = (*prog)[index];
		break;
	case 0x0001:
		/* inc X after */
		state->reg[rg] = (*prog)[index];
		inc_reg(14, state);
		break;
	case 0x0002:
		dec_reg(14, state);
		state->reg[rg] = (*prog)[--index];
		break;
	}

	state->ip += 2;
	return 0;
}
int ldi(uint16_t op, run_state *state)
{
	int rg = (op & 0x00f0) >> 4;
	state->reg[rg] = (op & 0x000f) | ((op & 0x0f00) >> 4);
//	printf("ldi ...\n");
	state->ip += 2;
	return 0;
}
int lds(uint16_t op, uint16_t op2, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	state->reg[rg] = (*prog)[op2];
	state->ip += 4;
	return 0;
}
int lsl(uint16_t op, run_state *state)
{
	uint16_t result, d;
	uint8_t c;
	d = (op & 0x00f0) >> 4;
	c = (state->reg[d] & 0x08);
	result = (state->reg[d] << 1);
	state->reg[d] = (uint8_t)result;
	if (c)
		state->sreg |= (c >> 7);
	else
		state->sreg &= ~C;
	/* V = N^C */
	check_zsnf(state->reg[d], state);
	state->sreg |= ((state->sreg ^ (state->sreg << 2)) & 4) << 1;
	state->ip += 2;
	return 0;
}
int lsr(uint16_t op, run_state *state)
{
	uint16_t result, d;
	uint8_t c;
	d = (op & 0x00f0) >> 4;
	c = (state->reg[d] & 0x01);
	result = (state->reg[d] >> 1);
	state->reg[d] = (uint8_t)result;
	if (c)
		state->sreg |= c;
	else
		state->sreg &= ~C;
	/* V = N^C */
	check_zsnf(state->reg[d], state);
	state->sreg |= ((state->sreg ^ (state->sreg << 2)) & 4) << 1;
	state->ip += 2;
	return 0;
}
int mov(uint16_t op, run_state *state)
{
	uint16_t r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	state->reg[d] = state->reg[r];
	state->ip += 2;
	return 0;
}
int movw(uint16_t op, run_state *state)
{
	uint16_t r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	/* shouldn't happen */
	if (r > 14 || d > 14)
		return 1;
	state->reg[d] = state->reg[r];
	state->reg[d+1] = state->reg[r+1];
	state->ip += 2;
	return 0;
	state->ip += 2;
	return 0;
}
int mul(uint16_t op, run_state *state)
{
	uint8_t c;
	uint16_t result, r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] * state->reg[r];
	state->reg[0] = (result & 0x00ff);
	state->reg[1] = (result & 0xff00) >> 8;
	c = state->reg[1] & 0x80;
	if (c)
		state->sreg |= C;
	else
		state->sreg &= ~C;
	if (!state->reg[0] && !state->reg[1])
		state->sreg |= Z;
	else
		state->sreg &= ~Z;

	state->ip += 2;
	return 0;
}
int muls(uint16_t op, run_state *state)
{
	uint8_t c;
	uint16_t result, r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = (int8_t)state->reg[d] * (int8_t)state->reg[r];
	state->reg[0] = (result & 0x00ff);
	state->reg[1] = (result & 0xff00) >> 8;
	c = state->reg[1] & 0x80;
	if (c)
		state->sreg |= C;
	else
		state->sreg &= ~C;
	if (!state->reg[0] && !state->reg[1])
		state->sreg |= Z;
	else
		state->sreg &= ~Z;

	state->ip += 2;
	return 0;
}
int neg(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	if (state->reg[d] != 0x80)
		state->reg[d] = ~state->reg[d] + 1;
	if (state->reg[d] == 0x80)
		state->sreg |= V;
	else
		state->sreg &= ~V;
	if (state->reg[d] == 0)
		state->sreg &= ~C;
	else
		state->sreg |= C;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int nop(uint16_t op, run_state *state)
{
	state->ip += 2;
	return 0;
}
int or(uint16_t op, run_state *state)
{
	uint16_t r, d;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	state->reg[d] = state->reg[d] | state->reg[r];
	/* v is cleared in this op */
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int ori(uint16_t op, run_state *state)
{
	uint16_t d = (op & 0x00f0) >> 4;
	state->reg[d] |= (op & 0x000f) | ((op & 0x0f00) >> 4);
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int pop(uint16_t op, run_state *state, uint8_t **prog)
{
	uint8_t d = (op & 0x00f0) >> 4;
	/* dont allow sp to go past ffff */
	state->reg[d] = pop_val(state, prog);
	state->ip += 2;
	return 0;
}
int push(uint16_t op, run_state *state, uint8_t **prog)
{
	uint8_t d = (op & 0x00f0) >> 4;
	push_val(state->reg[d], state, prog);
	state->ip += 2;
	return 0;
}
int ret(uint16_t op, run_state *state, uint8_t **prog)
{
	uint16_t h = pop_val(state, prog) << 8;
	uint8_t l = pop_val(state, prog);
	state->ip = h | l;
	return 0;
}
int ror(uint16_t op, run_state *state)
{
	uint16_t d;
	uint8_t c;
	d = (op & 0x00f0) >> 4;
	c = (state->reg[d] & 0x01); 
	state->reg[d] = (state->reg[d] >> 1) | (state->sreg << 7);
	if (c)
		state->sreg |= c;
	else
		state->sreg &= ~C;
	/* V = N^C */
	check_zsnf(state->reg[d], state);
	state->sreg |= ((state->sreg ^ (state->sreg << 2)) & 4) << 1;
	state->ip += 2;
	return 0;
}
int sbc(uint16_t op, run_state *state)
{
	int r,d;
	uint16_t c, result;
	c = state->sreg & C;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] - state->reg[r] - c;
	check_subf(result, state->reg[d], state->reg[r], state);
	state->reg[d] = (uint8_t)result;

	state->ip += 2;
	return 0;
}
int sbci(uint16_t op, run_state *state)
{
	int d;
	uint16_t c, result, val;
	c = state->sreg & C;
	d = (op & 0x00f0) >> 4;
	val = (op & 0x000f) | ((op & 0x00c0) >> 2);
	result = state->reg[d] - val - c;
	check_subf(result, state->reg[d], (uint8_t)val, state);
	state->reg[d] = (uint8_t)result;
	state->ip += 2;
	return 0;
}
int sbiw(uint16_t op, run_state *state)
{
	uint8_t rg = 0x08, m = 0, c = 0;
	uint16_t result;
	uint16_t val;
	m = (uint8_t)(op >> 3) & 0x06;
	rg |= m;
	val = (op & 0x000f) | ((op & 0x00c0) >> 2);
	result = state->reg[rg] - val;
	if (result & 0x0100)
		c = 1;
	state->reg[rg] = (uint8_t)result;
	result = state->reg[rg+1] - c;
	check_subf(result, state->reg[rg+1], c, state);
	state->reg[rg+1] = (uint8_t)result;
	/* check low byte for zero */
	if (state->reg[rg] && (state->sreg & Z))
		state->sreg &= ~Z;
	state->ip += 2;
	return 0;
}
int sbr(uint16_t op, run_state *state)
{
	int d;
	uint16_t val;
	d = (op & 0x00f0) >> 4;
	val = (op & 0x000f) | ((op & 0x00c0) >> 2);
	state->reg[d] |= val;
	state->sreg &= ~V;
	check_zsnf(state->reg[d], state);
	state->ip += 2;
	return 0;
}
int sbrc(uint16_t op, run_state *state, uint8_t **prog)
{
	uint8_t d = (op & 0x00f0) >> 4;
	uint8_t b = (op & 0x0007);
	if (!(state->reg[d] & (1 << b))) {
		state->ip += 2;
		if (check_32(state, prog))
			state->ip += 2;
	}
	state->ip += 2;
	return 0;
}
int sbrs(uint16_t op, run_state *state, uint8_t **prog)
{
	uint8_t d = (op & 0x00f0) >> 4;
	uint8_t b = (op & 0x0007);
	if ((state->reg[d] & (1 << b))) {
		state->ip += 2;
		if (check_32(state, prog))
			state->ip += 2;
	}
	state->ip += 2;
	return 0;
}
int setf(uint16_t op, run_state *state)
{
	uint8_t b = (op & 0x0070) >> 4;
	state->sreg |= (1 << b);
	state->ip += 2;
	return 0;
}
int ser(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	state->reg[d] = 0xff;
	state->ip += 2;
	return 0;
}
int stdx(uint16_t op, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	uint16_t iH = state->reg[13] << 8;
	uint16_t index = iH | state->reg[12];
	switch(op & 0x0003) {
	case 0x0000:
		/* no change to X */
		(*prog)[index] = state->reg[rg];
		break;
	case 0x0001:
		/* inc X after */
		(*prog)[index] = state->reg[rg];
		inc_reg(12, state);
		break;
	case 0x0002:
		dec_reg(12, state);
		(*prog)[--index] = state->reg[rg];
		break;
	}
	state->ip += 2;
	return 0;
}
int stdy(uint16_t op, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	uint16_t iH = state->reg[15] << 8;
	uint16_t index = iH | state->reg[14];
	switch(op & 0x0003) {
	case 0x0000:
		/* no change to X */
		(*prog)[index] = state->reg[rg];
		break;
	case 0x0001:
		/* inc X after */
		(*prog)[index] = state->reg[rg];
		inc_reg(14, state);
		break;
	case 0x0002:
		dec_reg(14, state);
		(*prog)[--index] = state->reg[rg];
		break;
	}
	state->ip += 2;
	return 0;
}
int sts(uint16_t op, uint16_t op2, run_state *state, uint8_t **prog)
{
	int rg = (op & 0x00f0) >> 4;
	(*prog)[op2] = state->reg[rg];
	state->ip += 4;
	return 0;
}
int sub(uint16_t op, run_state *state)
{
	int r,d;
	uint16_t result;
	r = op & 0x000f; d = (op & 0x00f0) >> 4;
	result = state->reg[d] - state->reg[r];
	check_subf(result, state->reg[d], state->reg[r], state);
	state->reg[d] = (uint8_t)result;
	state->ip += 2;
	return 0;
}
int subi(uint16_t op, run_state *state)
{
	int d;
	uint16_t result, val;
	d = (op & 0x00f0) >> 4;
	val = (op & 0x000f) | ((op & 0x00c0) >> 2);
	result = state->reg[d] - val;
	check_subf(result, state->reg[d], (uint8_t)val, state);
	state->reg[d] = (uint8_t)result;
	state->ip += 2;
	return 0;
}
int swap(uint16_t op, run_state *state)
{
	uint8_t d = (op & 0x00f0) >> 4;
	uint8_t h = state->reg[d] & 0xf0;
	state->reg[d] = (state->reg[d] << 4) | h;
	state->ip += 2;
	return 0;
}

int execute(run_state *state, uint8_t **prog, _win *w)
{
	uint16_t op, op2;
	int err = 0;
	/* stack pointer */
	state->sp = 0xffff;
	while (1) {
#if DEBUG

		clock_t begin = clock();
#endif
		/* lsb */
		op = (*prog)[state->ip] | ((*prog)[(state->ip)+1]) << 8;
//		printf("op: %04x\n", op);
//		printf("mask: %04x\n", DRD_MSK(op));
		if (DRD_MSK(op) == 0x1c00) {
			err = adc(op, state);
		} else
		if (DRD_MSK(op) == 0x0c00) {
			err = add(op, state);
		} else
		if (IMP_MSK(op) == 0x9598) {
			err = abrk(op, state);
		} else
		if (DRD_MSK(op) == 0x9600) {
			err = adiw(op, state);
		} else 
		if (DRD_MSK(op) == 0x2000) {
			err = and(op, state);
		} else 
		if (IMM_MSK(op) == 0x7000) {
			err = andi(op, state);
			/* special case for cbr */
		} else
		if (DRS_MSK(op) == 0x9405) {
			err = asr(op, state);
		} else 
		if (BIT_MSK(op) == 0x9488) {
			err = bclr(op, state);
		} else 
		if (LSB_MSK(op) == 0xf800) {
			err = bld(op, state);
		} else 
		if (BRCH_MSK(op) == 0xf400) {
			err = branchc(op, state);
		} else 
		if (BRCH_MSK(op) == 0xf000) {
			err = branchs(op, state);
		} else 
		if (BIT_MSK(op) == 0x9408) {
			err = bset(op, state);
		} else 
		if (LSB_MSK(op) == 0xfa00) {
			err = bst(op, state);
		} else 
		if (IMM_MSK(op) == 0xc000) {
			err = cbr(op, state);
		} else 
		if (IMP_MSK(op) == 0x940e) {
			op2 = (*prog)[(state->ip)+2] | ((*prog)[(state->ip)+3]) << 8;
			err = call(op, op2, state, prog);
		} else 
			/*
		if (DRS_MSK(op) == 0x9408) {
			err = clrf(op, state);
		} else
		*/
		if (DRS_MSK(op) == 0x9400) {
			err = com(op, state);
		} else
		if (DRD_MSK(op) == 0x1400) {
			err = cp(op, state);
		} else
		if (DRD_MSK(op) == 0x0400) {
			err = cpc(op, state);
		} else
		if (IMM_MSK(op) == 0x3000) {
			err = cpi(op, state);
		} else
		if (DRD_MSK(op) == 0x1000) {
			err = cpse(op, state, prog);
		} else
		if (DRS_MSK(op) == 0x940a) {
			err = dec(op, state);
		} else
		if (DRD_MSK(op) == 0x2400) {
			err = eor(op, state);
		} else
		if (IMP_MSK(op) == 0x9509) {
			err = icall(op, state, prog);
		} else
		if (IMP_MSK(op) == 0x9409) {
			err = ijmp(op, state, prog);
		} else
		if (DRS_MSK(op) == 0x9403) {
			err = inc(op, state);
		} else
		if (IMP_MSK(op) == 0x940c) {
			op2 = (*prog)[(state->ip)+2] | ((*prog)[(state->ip)+3]) << 8;
			err = jmp(op, op2,  state);
		} else
		if (LDD_MSK(op) == 0x900c) {
			err = lddx(op, state, prog);
		} else
		if (LDD_MSK(op) == 0x9008) {
			err = lddy(op, state, prog);
		} else
		if (IMM_MSK(op) == 0xe000) {
			err = ldi(op, state);
		} else
		if (DRS_MSK(op) == 0x9000) {
			op2 = (*prog)[(state->ip)+2] | ((*prog)[(state->ip)+3]) << 8;
			err = lds(op, op2, state, prog);
		} else
		if (DRD_MSK(op) == 0x0c00) {
			err = lsl(op, state);
		} else
		if (DRS_MSK(op) == 0x9406) {
			err = lsr(op, state);
		} else
		if (DRD_MSK(op) == 0x2c00) {
			err = mov(op, state);
		} else
		if (DRD_MSK(op) == 0x0100) {
			err = movw(op, state);
		} else
		if (DRD_MSK(op) == 0x9c00) {
			err = mul(op, state);
		} else
		if (DRD_MSK(op) == 0x0200) {
			err = muls(op, state);
		} else
		if (DRS_MSK(op) == 0x9401) {
			err = neg(op, state);
		} else
		if (IMP_MSK(op) == 0x0000) {
			err = nop(op, state);
		} else
		if (DRD_MSK(op) == 0x2800) {
			err = or(op, state);
		} else
		if (IMM_MSK(op) == 0x6000) {
			err = ori(op, state);
		} else
		if (DRS_MSK(op) == 0x9203) {
			err = pop(op, state, prog);
		} else
		if (DRS_MSK(op) == 0x920f) {
			err = push(op, state, prog);
		} else
		if (IMP_MSK(op) == 0x9508) {
			err = ret(op, state, prog);
		} else
			/*rol is adc rd,rd */
		if (DRS_MSK(op) == 0x9407) {
			err = ror(op, state);
		} else
		if (DRD_MSK(op) == 0x0800) {
			err = sbc(op, state);
		} else
		if (IMM_MSK(op) == 0x4000) {
			err = sbci(op, state);
		} else
		if (DRD_MSK(op) == 0x9700) {
			err = sbiw(op, state);
		} else
		if (IMM_MSK(op) == 0x6000) {
			err = sbr(op, state);
		} else
		if (LSB_MSK(op) == 0xfc00) {
			err = sbrc(op, state, prog);
		} else
		if (LSB_MSK(op) == 0xfe00) {
			err = sbrs(op, state, prog);
		} else
			/* should be bset
		if (DRS_MSK(op) == 0x9408) {
			err = setf(op, state);
		} else
		*/
		if (DRS_MSK(op) == 0xef0f) {
			err = ser(op, state);
		} else
		if (LDD_MSK(op) == 0x920c) {
			err = stdx(op, state, prog);
		} else
		if (LDD_MSK(op) == 0x9208) {
			err = stdy(op, state, prog);
		} else
		if (DRS_MSK(op) == 0x9200) {
			op2 = (*prog)[(state->ip)+2] | ((*prog)[(state->ip)+3]) << 8;
			err = sts(op, op2, state, prog);
		} else
		if (DRD_MSK(op) == 0x1800) {
			err = sub(op, state);
		} else
		if (IMM_MSK(op) == 0x6000) {
			err = subi(op, state);
		} else
		if (DRS_MSK(op) == 0x9402) {
			err = swap(op, state);
		} else {
			printf("didn't recognize op: %02x\n", op);
			break;
		}
		
		/* scan screen addresses and display */
		/* print reg sreg ip */
		if (err > 0)
			break;
		err = run_io(op, state, prog, w);

		/*
		printf("op %04x sreg %02x ip:%02x sp:%02x r0:%02x r1:%02x r2:%02x "
			"r3:%02x r4:%02x r5:%02x r6:%02x r7:%02x \nr8:%02x r9:%02x "
			"r10:%02x r11:%02x r12:%02x r13:%02x r14:%02x r15:%02x\n\n", op, 
			state->sreg, state->ip, state->sp,
			state->reg[0], state->reg[1], state->reg[2], state->reg[3], 
			state->reg[4], state->reg[5], state->reg[6], state->reg[7], 
			state->reg[8], state->reg[9], state->reg[10], state->reg[11], 
			state->reg[12], state->reg[13], state->reg[14], state->reg[15]);
			*/
		usleep(1000);
		/* sleep ? */
		if (err < 0)
			break;
#if DEBUG
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		mvwprintw(w->text, 0,0, "clock: %f\n", time_spent);
		wrefresh(w->text);
#endif
	}
	return err;
}
