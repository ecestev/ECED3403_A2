/****************************************************************************************
*	FILE: twoaddrinst.cpp
*
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*
*				Last Modified	Author
*	ORIGINAL:	June 2015		Dr. Hughes
*	MODIFIED:	July 2017		Stephen Sampson
*
*	This file contains One address instructions
*		Basic (common) operations:
*			1. Get src operand (determined by As and SReg)
*			2. Get dst operand (determined by Ad and Dreg)
*			3. Manipulate operand according to instruction
*			4. Put operand (determined by Ad and DReg)
****************************************************************************************/

#include "globals.h"
#include "machine.h"
#include <iostream>
#include <bitset>


using namespace std;

uint32_t result;
uint16_t srcop;
uint16_t dstop;

void ta_not_implemented(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg)
{
	cout << "Not implemented two-addr instruction PC: " << registers[PC] << endl;
}

void mov(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src --> dst
	// status bits not affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "MOV";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = 0;			// DIAG OUTPUT IF DEBUG MODE
		last_result = 0;		// DIAG OUTPUT IF DEBUG MODE
	#endif	
	put_operand(ad, dreg, srcop);

}

void add(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src + dst --> dst
	// all status bits affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "ADD";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = dstop + srcop;
	handler_carry(result, bw);
	handler_negative(result, bw);
	handler_overflow(srcop, dstop, result, bw);
	handler_zero(result, bw);
	dstop = result;
	put_operand(ad, dreg, dstop);
	
	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void addc(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// scr + dst + C --> dst
	// all status bits affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "ADDC";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = dstop + srcop + srptr->carry;
	handler_carry(result, bw);
	handler_negative(result, bw);
	handler_overflow(srcop, dstop, result, bw);
	handler_zero(result, bw);
	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG
	#endif
}

void subc(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// dst + .NOT.src + C --> dst
	// all status bits affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "SUBC";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif	

	srcop = ~srcop;
	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = dstop + srcop + srptr->carry;
	handler_carry(result, bw);
	handler_negative(result, bw);
	handler_overflow(srcop, dstop, result, bw);
	handler_zero(result, bw);
	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void sub(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// dst + .NOT.src + 1 --> dst
	// all status bits affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "SUB";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	srcop = ~srcop;
	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = dstop + srcop + 1;
	handler_carry(result, bw);
	handler_negative(result, bw);
	handler_overflow(srcop, dstop, result, bw);
	handler_zero(result, bw);
	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void cmp(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// dst + .NOT.src + 1 
	// result is not stored. only sr bits affected

	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "CMP";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	srcop = ~srcop;
	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = dstop + srcop + 1;
	handler_carry(result, bw);
	handler_negative(result, bw);
	handler_overflow(srcop, dstop, result, bw);
	handler_zero(result, bw);

	#ifdef DEBUG
		dstop = result;			// DIAG OUTPUT IF DEBUG MODE
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}


void bit(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src .AND. dst
	// only status bits are affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "BIT";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = srcop & dstop;
	handler_negative(result, bw);
	handler_zero(result, bw);
	if (srptr->zero != SET)
		srptr->carry = SET;
	else
		srptr->carry = CLR;
	srptr->overflow = 0;

	#ifdef DEBUG
		dstop = result;			// DIAG OUTPUT IF DEBUG MODE
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void bic(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// .NOT.src .AND. dst → dst
	// status bits are not affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "BIC";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	srcop = ~srcop;
	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = srcop & dstop;
	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void bis(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src .OR. dst → dst
	// status bits not affected
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "BIS";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}
	result = srcop | dstop;
	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void xor(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src .XOR. dst → dst
	// all status bits are affected 
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "XOR";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif
	
	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}

	result = srcop ^ dstop;
	handler_negative(result, bw);
	handler_zero(result, bw);
	if (srptr->zero != SET)
		srptr->carry = SET;
	else
		srptr->carry = CLR;
	if (((srcop & 0x8000) == 1) && ((dstop & 0x8000) == 1))
		srptr->overflow = SET;
	dstop = result;
	put_operand(ad, dreg, dstop);
	
	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void and(unsigned bw, unsigned as, unsigned sreg, unsigned ad, unsigned dreg) {
	// src .AND. dst → dst
	// all status bits affected (overflow cleared)
	srcop = get_operand(as, sreg);
	dstop = get_operand(ad, dreg);

	#ifdef DEBUG
		last_inst = "AND";		// DIAG OUTPUT IF DEBUG MODE
		last_srcop = srcop;		// DIAG OUTPUT IF DEBUG MODE
		last_dstop = dstop;		// DIAG OUTPUT IF DEBUG MODE
	#endif

	if (bw == BYTE) {
		srcop &= 0x00FF;
		dstop &= 0X00FF;
	}

	result = srcop & dstop;
	handler_negative(result, bw);
	handler_zero(result, bw);
	if (srptr->zero != SET)
		srptr->carry = SET;
	else
		srptr->carry = CLR;
	srptr->overflow = CLR;

	dstop = result;
	put_operand(ad, dreg, dstop);

	#ifdef DEBUG
		last_result = dstop;	// DIAG OUTPUT IF DEBUG MODE
	#endif
}

