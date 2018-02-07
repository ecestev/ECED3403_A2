/****************************************************************************************
*	FILE: oneaddrinst.cpp
*
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*
*				Last Modified	Author
*	ORIGINAL:	June 2016		Dr. Hughes
*	MODIFIED:	July 2017		Stephen Sampson
*
*	This file contains one address instructions
*		Basic (common) operations:
*			1. Get operand (determined by As and Reg)
*			2. Execute instruction and store result in operand
*			3. Put operand (determined by As and Reg)
****************************************************************************************/

#include "globals.h"
#include "machine.h"
#include <bitset>
#include <iostream>

#define LSBYTE(x)   ((x)&0x00ff)
#define MSBYTE(x)   (((x)&0xff00)>>8)
#define LSBIT(x)   ((x)&0x0001)
#define MSBIT(x)   ((x)&0x8000)

using namespace std;

uint16_t operand;

void oa_not_implemented(unsigned bw, unsigned as, unsigned regnum) {
	#ifdef DEBUG
		last_inst = "Not implemented instruction PC: ";
		last_srcop = -1;
		last_dstop = -1;
		last_result = -1;
	#endif
}

void rrc(unsigned bw, unsigned as, unsigned regnum) {
	// ROTATE RIGHT THROUGH CARRY
	// C → MSB → MSB-1 .... LSB+1 → LSB → C
	// NOTE: Overflow cleared, other SR bits affected
	operand = get_operand(as, regnum);
	if (bw == BYTE)
		operand &= 0x00FF;

	#ifdef DEBUG
		last_inst = "RRC";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	bool carry = false;
	if (srptr->carry == SET)
		carry = true;
	srptr->carry = LSBIT(operand);
	operand = operand >> 1;
	if (bw == BYTE) {
		if (carry == SET)
			operand |= 1 << 7;
		operand = LSBYTE(operand);
	} else {
		if (carry == SET)
			operand |= 1 << 15;
	}
	if (carry == true)
		srptr->negative = SET;
	srptr->overflow = CLR;
	if (operand == 0)
		srptr->zero = SET;
	else
		srptr->zero = CLR;
	put_operand(as, regnum, operand);

	#ifdef DEBUG
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void swpb(unsigned bw, unsigned as, unsigned regnum) {
	// SWAP BYTES (BW IS IGNORED)
	// Bits 15 to 8 ↔ bits 7 to 0
	// NOTE: SR not affected
	operand = get_operand(as, regnum);
	
	#ifdef DEBUG
		last_inst = "SWPB";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	operand = (LSBYTE(operand) << 8) | (MSBYTE(operand)); 
	put_operand(as, regnum, operand);

	#ifdef DEBUG
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void rra(unsigned bw, unsigned as, unsigned regnum) {
	// ROTATE RIGHT ARITHMETICALLY
	// MSB → MSB, MSB → MSB-1, ... LSB+1 → LSB, LSB → C
	// NOTE: Overflow cleared, other SR bits affected
	operand = get_operand(as, regnum);
	
	if (bw == BYTE)
		operand &= 0x00FF;

	#ifdef DEBUG
		last_inst = "RRA";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif
	
	srptr->carry = LSBIT(operand);
	operand = operand >> 1;

	if (bw == BYTE) {
		// set the 7th bit of 'operand' equal to the 6th bit of 'operand'
		operand ^= (-((operand >> 6) & 1) ^ operand) & (1 << 7);
		if (GET_BIT(operand, 7) == 1)
			srptr->negative = SET;
		else
			srptr->negative = CLR;
	} else {
		// set the 15th bit of 'operand' equal to the 14th bit of 'operand'
		operand ^= (-((operand >> 14) & 1) ^ operand) & (1 << 15);
		if (GET_BIT(operand, 15) == 1)
			srptr->negative = SET;
		else
			srptr->negative = CLR;
	}

	srptr->overflow = CLR;
	if (operand == 0)
		srptr->zero = SET;
	else
		srptr->zero = CLR;
	put_operand(as, regnum, operand);

	#ifdef DEBUG
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void sxt(unsigned bw, unsigned as, unsigned regnum) {
	// EXTEND SIGN
	// Bit 7 → Bit 8 ......... Bit 15
	// NOTE: Overflow cleared, other SR bits affected
	operand = get_operand(as, regnum);

	#ifdef DEBUG
		last_inst = "SXT";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	operand = static_cast<int16_t>(static_cast<int8_t>(operand));
	srptr->overflow = CLR;
	if (operand == 0) {
		srptr->zero = SET;
		srptr->carry = CLR;
	} else {
		srptr->zero = CLR;
		srptr->carry = SET;
	}
	if (GET_BIT(operand, 15) == 1)
		srptr->negative = SET;
	put_operand(as, regnum, operand);

	#ifdef DEBUG
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void push(unsigned bw, unsigned as, unsigned regnum) {
	// PUSH BYTE/WORD ONTO STACK
	// SP - 2 -> SP ; src -> @SP
	// NOTE: SR NOT AFFECTED
	operand = get_operand(as, regnum);
	if (bw == BYTE)
		operand &= 0x00FF;

	#ifdef DEBUG
		last_inst = "PUSH";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	registers[SP] -= 2;
	sys_clk++;
	MBR_WR(registers[SP], operand, WORD);
	put_operand(as, regnum, operand);
}

void call(unsigned bw, unsigned as, unsigned regnum) {
	// NOTE: SR NOT AFFECTED
	operand = get_operand(as, regnum);
		
	#ifdef DEBUG
		last_inst = "CALL";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = operand;		// DIAG OUTPUT IF DEBUG MODE
		last_result = operand;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	registers[SP] -= 2;
	memory[registers[SP]] = LSBYTE(registers[PC]);
	memory[registers[SP]+1] = MSBYTE(registers[PC]);
	registers[PC] = operand;
	put_operand(as, regnum, operand);
}

void reti(unsigned bw, unsigned as, unsigned regnum) {
	// NOTE: All SR bits affected
	#ifdef DEBUG
		last_inst = "RETI";			// DIAG OUTPUT IF DEBUG MODE
		last_srcop = 0;				// DIAG OUTPUT IF DEBUG MODE
		last_result = 0;			// DIAG OUTPUT IF DEBUG MODE
	#endif

	MBR_RD(registers[SP], registers[SR], WORD);
	registers[SP] += 2;
	MBR_RD(registers[SP], registers[PC], WORD);
	registers[SP] += 2;
	
	#ifdef DEBUG
		cout << endl;
		cout << "-----------------------------------" << endl;
		cout << "INTERRUPT SERVICE ROUTINE COMPLETE!" << endl;
		cout << "-----------------------------------" << endl;
	#endif
}

