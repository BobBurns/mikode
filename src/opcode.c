#include "common.h"
#include "instructions.h"

static struct {
	char name[16];
	int type;
	uint16_t opcode;
}ins[MAX_INSTR] = {
	{".db", DB_T, 0x0000},
	{".dw", DB_T, 0x0000},
	{".equ", DB_T, 0x0000},
	{"AddWithCarry", DIR_DUAL, 0x1c00},
	{"Add", DIR_DUAL, 0x0c00},
	{"AddImmWord", IMM_WORD, 0x9600},
	{"And", DIR_DUAL, 0x2000},
	{"AndImm", IMM_BYTE, 0x7000},
	{"ArithShiftRt", DIR_SING, 0x9405},
	{"BitLoad", LDSTR_BIT, 0xf800}, /* bit load from t flag to bit in reg */
	{"BitClear", BIT, 0x9488}, /* bit clear in sreg  s =< 7 */
	{"BranchBitClr", BR_BIT, 0xf400}, /* branch if bit clear in sreg */
	{"BranchBitSet", BR_BIT, 0xf000}, /* branch if bit set in sreg */
	{"BranchCarryClr", BRANCH, 0xf400}, /* branch if carry clear */
	{"BranchCarrySet", BRANCH, 0xf000}, /* branch if carry set */
	{"Break", IMPLIED, 0x9598},
	{"BranchEqu", BRANCH, 0xf001}, /* branch on equal z=0 */
	{"BranchGrEqu", BRANCH, 0xf404}, /* branch if greater or equal */
	{"brid", BRANCH, 0xf407}, /* branch if global interupt is disabled */
	{"brie", BRANCH, 0xf007}, /* branch if glogan interupt is enabled */
	{"BranchLower", BRANCH, 0xf000}, /* branch if lower unsigned */
	{"BranchLessThan", BRANCH, 0xf004},
	{"BranchMinus", BRANCH, 0xf002},
	{"BranchNotEqu", BRANCH, 0xf401},
	{"BranchPlus", BRANCH, 0xf402},
	{"BranchSameHigh", BRANCH, 0xf400},
	{"BranchTClr", BRANCH, 0xf406}, /* branch if t flag is cleared */
	{"BranchTSet", BRANCH, 0xf006}, /* branch if t flag is set */
	{"BranchOverClr", BRANCH, 0xf403},
	{"BranchOverSet", BRANCH, 0xf003},
	{"BitSetSreg", BIT, 0x9408},
	{"BitStoreT", LDSTR_BIT, 0xfa00}, /* bit store from reg to t flag */
	{"Call", DIR_32, 0x940e}, /* call 16 bit address */
	/* cbr hijacking rjmp */
	{"ClearBitsReg", IMM_BYTE, 0xc000}, /* clear bits in register */
	{"ClearCarryF", IMPLIED, 0x9488}, /* clear carry flag */
	/* not implementing half carry */
	{"cli", IMPLIED, 0x94f8}, /* clear global interrup flag */
	{"ClearNegF", IMPLIED, 0x94a8}, /* clear negative flag */
	{"ClearReg", DIR_STD, 0x2400}, /* clear register eor rd */
	{"ClearSignF", IMPLIED, 0x94c8}, /* clear signed flag */
	{"ClearTranF", IMPLIED, 0x94e8}, /* clear transfer flag */
	{"ClearOverflowF", IMPLIED, 0x94b8}, /* clear overflow flag */
	{"ClearZeroF", IMPLIED, 0x9498}, /* clear zero flag */
	{"Compliment", DIR_SING, 0x9400}, /* one's compliment */
	{"Compare", DIR_DUAL, 0x1400}, /* compare  */
	{"CompareCarry", DIR_DUAL, 0x0400}, /* compare with carry  */
	{"CompareImm", IMM_BYTE, 0x3000}, /* compare immediate  */
	{"CompareSkipEqu", DIR_DUAL, 0x1000}, /* compare skip if equal  */
	{"Decrement", DIR_SING, 0x940a}, /* decrement  */
	{"ExclusiveOr", DIR_DUAL, 0x2400}, /* exclusive or  */
	/* not implementing fmul */
	{"IndCall", IMPLIED, 0x9509},
	{"IndJump", IMPLIED, 0x9409}, /* indirect jump z reg? */
	{"Increment", DIR_SING, 0x9403}, /* increment */
	{"Jump", DIR_32, 0x940c}, /* increment */
	/* not implementing las lac lat */
	{"LoadIndX", LD_IND, 0x900c}, /* load indirect x -X +X */
	{"LoadIndY", LD_IND, 0x9008}, /* load indirect Y -Y +Y */
	/* not implemeting Z or displacement */
	{"LoadImm", IMM_BYTE, 0xe000}, /* load immediate */
	{"LoadDirect", LD_DIR, 0x9000}, /* load direct from data space */
	{"LogicalShiftRt", DIR_SING, 0x9406}, /* logical shift right */
	/* using top 15 reg for opcode instead of add, rd, rd */
	{"LogicalShiftLft", DIR_STD, 0x0c00}, /* logical shift left  same as add rd,rd*/
	{"Move", DIR_DUAL, 0x2c00}, /* copy register */
	{"MoveWord", DIR_WORD, 0x0100}, /* copy register word Rd+1:Rd,Rr+1:Rr*/
	{"Multiply", DIR_DUAL, 0x9c00}, /* multiply unsigned */
	{"MultiplyS", DIR_DUAL, 0x0200}, /* multiply signed product placed in R1:R0*/
	{"Negate", DIR_SING, 0x9401}, /* two's compliment */
	{"NOP", IMPLIED, 0x0000}, /* no operation */
	{"Or", DIR_DUAL, 0x2800}, /* logical or */
	{"OrImm", IMM_BYTE, 0x6000}, /* lgical or with immediate */
	{"Pop", DIR_SING, 0x9203}, /* pop register from stack */
	{"Push", DIR_SING, 0x920f}, /* push register on stack */
	/* not implementing relative jump/call */
	{"Return", IMPLIED, 0x9508}, /* return from subroutine */
	/* again using top 15 register bit instead of add */
	{"RotateLeft", DIR_STD, 0x1c00}, /* rotate left through carry */
	{"RotateRight", DIR_SING, 0x9407}, /* rotate right through carry */
	{"SubtractCarry", DIR_DUAL, 0x0800}, /* subtract with carry */
	{"SubtractImmC", IMM_BYTE, 0x4000}, /* subtract immediate with carry */
	/* not implemeting i/o register codes eg sbic */
	{"SubtractImmWord", IMM_WORD, 0x9700}, /* subtract immediate from word */
	{"SetBitsReg", IMM_BYTE, 0x6000}, /* set bits in register Rd, K */
	{"SkipBitRegClr", LDSTR_BIT, 0xfc00}, /* skip if bit in register is clear */
	{"SkipBitRegSet", LDSTR_BIT, 0xfe00}, /* skip if bit in register is set */
	{"SetCarryF", IMPLIED, 0x9408}, /* set carry flag */
	{"sei", IMPLIED, 0x9478}, /* set global interupt flag */
	{"SetNegF", IMPLIED, 0x9428}, /* set negative flag */
	{"SetAllReg", DIR_SING, 0xef0f}, /* set all bits in register */
	{"SetSignF", IMPLIED, 0x9448}, /* set signed flag */
	{"SetTranF", IMPLIED, 0x9468}, /* set transfer flag */
	{"SetOverflowF", IMPLIED, 0x9438}, /* set overflow flag */
	{"SetZeroF", IMPLIED, 0x9418}, /* set zero flag */
	{"StoreIndX", STR_IND, 0x920c}, /* store indirect x */
	{"StoreIndY", STR_IND, 0x9208}, /* store indirect x */
	{"StoreDirect", STR_DIR, 0x9200}, /* store direct to data space */
	{"Subtract", DIR_DUAL, 0x1800}, /* subtract without carry */
	{"SubtractImm", IMM_BYTE, 0x5000}, /* subtract immediate */
	{"Swap", DIR_SING, 0x9402}, /* swap nibbles */
	{"TestZero", DIR_STD, 0x2000} /* test for zero or minus */
};


int getopcode(line *in, char *b)
{
	if (DEBUG)
		printf("getopcode\n");
	uint8_t i;
	for ( i = 0; i < MAX_INSTR; i++)
	{
		/* TODO add type to line */
		if (strncasecmp(b,ins[i].name, MAX_TOK) == 0) 
		{
			in->opcode = ins[i].opcode;
			in->type = ins[i].type;
			return 0;
		}
	}
	return -1;
}
