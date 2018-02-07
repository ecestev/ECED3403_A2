/******************************************************************************
Machine Header File
ECED 3403 - Computer Architecture
Stephen Sampson - B00568374
July 2017
******************************************************************************/

#pragma once
#include <string>

int machine();
uint16_t fetch();
uint16_t get_word(int loc);
void handler_carry(uint32_t &result, unsigned bw);
void handler_negative(uint32_t &result, unsigned bw);
void handler_overflow(uint16_t &srcop, uint16_t &dstop, uint32_t &result, unsigned bw);
void handler_zero(uint32_t &result, unsigned bw);

enum MACHINE_RTN_CODES { MACHINE_DONE };

/* Global terms */
#define TRUE		1			
#define FALSE		0			
#define SET			1			
#define CLR			0			
#define PRIVATE		static		
#define MAX_MEM_SZ	65536		

/* Memory control */
#define WORD      0				
#define BYTE      1				
#define READ      0				
#define WRITE     1				

#define MEM_ACCESS_TIME 10		// Update sys_clk on each bus rd/wr */

/* Register file */
#define PC        0				// Program Counter - R0
#define SP        1				// Stack Pointer - R1
#define SR        2				// Status register - R2
#define CG1       SR			// Constant generator 1 - R2
#define CG2       3				// Constant generator 2 - R3
#define TOS		  0xFFC0		// Top of stack

/* Devices */
#define NUM_DEVICES  16			// Number of devices supported by mcu 
#define DEVICE_LIMIT 32			// SCR:DR pairs - 0 to DEV_LIM-1 (low mem) 
#define VECTOR_BASE  0xFFC0		// Start of interrupt vector (high mem) 

/* SR bits */
struct sr_bits
{
	unsigned
		carry : 1,				
		zero : 1,				
		negative : 1,			
		gie : 1,				
		coff : 1,				
		ooff : 1,				
		scg0 : 1,				
		scg1 : 1,				
		overflow : 1,			
		reserved : 7;			
};

/* One address entry points */
extern void oa_not_implemented(unsigned bw, unsigned as, unsigned regnum);
extern void rrc(unsigned bw, unsigned as, unsigned regnum);
extern void swpb(unsigned bw, unsigned as, unsigned regnum);
extern void rra(unsigned bw, unsigned as, unsigned regnum);
extern void sxt(unsigned bw, unsigned as, unsigned regnum);
extern void push(unsigned bw, unsigned as, unsigned regnum);
extern void call(unsigned bw, unsigned as, unsigned regnum);
extern void reti(unsigned bw, unsigned as, unsigned regnum);

/* Two address entry points */
extern void ta_not_implemented(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void mov(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void add(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void addc(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void subc(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void sub(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void cmp(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void dadc(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void bit(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void bic(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void bis(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void xor(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);
extern void and(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg);

/* Jump entry points */
extern void jmp_not_implemented(unsigned offset);
extern void jng(unsigned offset);
extern void jne(unsigned offset);
extern void jeq(unsigned offset);
extern void jnc(unsigned offset);
extern void jc(unsigned offset);
extern void jn(unsigned offset);
extern void jge(unsigned offset);
extern void jl(unsigned offset);
extern void jmp(unsigned offset);

/* addressing mode entry points */
extern unsigned short get_operand(unsigned, unsigned);
extern void put_operand(unsigned, unsigned, unsigned short);

/* device entry points (from bus|main only) */
extern void init_devices();
extern void check_for_status_change();
extern void dev_mem_access(unsigned short, unsigned char *, unsigned);

/* bus entry point */
extern void bus(unsigned short, unsigned short *, unsigned, unsigned);

/* memory data register macros */
#define MBR_RD(addr, data, bw)		bus((addr), (&data), READ,	(bw))
#define MBR_WR(addr, data, bw)		bus((addr), (&data), WRITE, (bw))

/* common bit operatiosn */
#define GET_BIT(number, bit)		((number >> bit) & 1)
#define SET_BIT(number, bit)		(number |= 1 << bit)
#define CLR_BIT(number, x)			number &= ~(1 << x)

/* increment program counter by a word */
#define INC_PC	registers[PC]+=2
