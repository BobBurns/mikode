/* defines for instruction types */

#define DIR_SING 18 /* register direct Rd*/
#define DIR_DUAL 17 /* register direct dual Rd,Rr */
#define DIR_STD 16 /* register direct single to dual .used to translate intructions*/
#define IMM_BYTE 15 /* regiter immediate byte Rd,K */
#define IMM_WORD 14 /* register word from  immediate, Rd+1:Rd, K ADIW */
#define BRANCH 13 /* intruction k += 64  7bits (label) */
#define BR_BIT 12 /* intruction k += 64  7bits (label) sss,kkkkkkk */
#define DIR_32  11 /* 32 bit Opcode 16 bit address k */
#define IMPLIED 10 /* No operand */
#define LD_IND 9 /* indirect addressing X -X X+ and Y */
#define STR_IND 8 /* indirect addressing X -X X+ and Y */
#define LD_DIR 7 /* direct addressing 32 bit opcode */
#define STR_DIR 6 /* direct addressing 32 bit opcode */
#define SKIP 5 /* Rr, b < 8 */
#define BIT 4 /* bit s < 8 */
#define LDSTR_BIT 3 /*load bit Rd, s < 8 */
#define DB_T 2 /* data byte */
#define DIR_BIT 1 /* direct bit Rd, b */
#define DIR_WORD 0 /* direct word Rh:Rl,Rh:rl */
#define MAX_INSTR 110

