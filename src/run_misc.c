#include "runheader.h"


int check_zsnf(uint8_t r, run_state *state)
{
	if (r == 0)
		state->sreg |= Z;
	else
		state->sreg &= ~Z;
	if (r & 0x80)
		state->sreg |= N;
	else
		state->sreg &= ~N;

	if ((state->sreg & N) ^ ((state->sreg & V)>>1))
		state->sreg |= S;
	else 
		state->sreg &= ~S;
	return 0;
}

int check_cv(uint16_t result, uint8_t a, uint8_t b, run_state *state)
{
	if (result & 0x0100) 
		state->sreg |= C;
	else
		state->sreg &= ~C;
	/* check overflow */
	/*
	if ((a^(uint8_t)result)&(b^(uint8_t)result)&0x80)
		state->sreg |= V;
		*/
	if (((a & b & ~result) | (~a & ~b & result)) & 0x80)
		state->sreg |= V;
	else 
		state->sreg &= ~V;
	return 0;
}

int check_subf(uint16_t result, uint8_t a, uint8_t b, run_state *state)
{
	if (((a & ~b & ~(uint8_t)result) | (~a & b & (uint8_t)result)) & 0x80)
		state->sreg |= V;
	else
		state->sreg &= ~V;
	if (result)
		state->sreg &= ~Z;
	if (result & 0x80)
		state->sreg |= N;
	else
		state->sreg &= ~N;

	if ((state->sreg & N) ^ ((state->sreg & V)>>1))
		state->sreg |= S;
	else 
		state->sreg &= ~S;
	if ((b + (state->sreg & 0x01)) > a)
		state->sreg |= C;
	else
		state->sreg &= ~C;
	return 0;
}

int check_32(run_state *state, uint8_t **prog)
{
	uint16_t op;
	op = (*prog)[state->ip] | ((*prog)[(state->ip)+1]) << 8;
	/* call jmp sts lds */
	if (op == 0x940e || op == 0x940c || DRS_MSK(op) == 0x9200 || DRS_MSK(op) == 0x9000)
		return 1;
	return 0;
}

uint8_t pop_val(run_state *state, uint8_t **prog)
{
	/* dont allow sp to go past ffff */
	if (++state->sp == 0x0000)
		state->sp = 0xffff;
	return  (uint8_t)(*prog)[state->sp];
}
int push_val(uint8_t val, run_state *state, uint8_t **prog)
{
	(*prog)[state->sp] = val;
	/* dont allow sp to go past e000 */
	if (--state->sp == 0xf000)
		state->sp = 0xf000;
	return 0;
}

int inc_reg(uint8_t rg, run_state *state)
{
	if (++(state->reg[rg]) == 0)
		state->reg[rg+1]++;
	return 0;
}

int dec_reg(uint8_t rg, run_state *state)
{
	if (--(state->reg[rg]) == 0)
		state->reg[rg+1]--;
	return 0;
}
