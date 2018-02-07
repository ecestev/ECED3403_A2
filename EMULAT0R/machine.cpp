/****************************************************************************************
*	FILE: machine.cpp
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
*	This file contains:
*		The fetch and decode operations for the TI 430 emulator
*		NOTE: any executable should be loaded at this point. Function
*		to execute instruction is called once decoded.
****************************************************************************************/

#include <windows.h>
#include "globals.h"
#include "machine.h"
#include <iostream>
#include <bitset>
#include <functional>
#include <vector>

using namespace std;

// Reset state register values
#define RESET_PC        0xFFFE
#define RESET_SR        0

// Macros to extract specific bits from different instruction types
// Extract opcode type (common to one op, two op, and jumps)
#define OPCODE_TYPE(x)		((x)>>13)
// One op masks
#define OA_REG(x)			((x)&0x0f)
#define OA_AS(x)			(((x)>>4)&0x03)
#define OA_BW(x)			(((x)>>6)&0x01)
#define OA_MASK(x)			(((x)>>7)&0x07)
// Jump masks
#define JP_OFF(x)			((x)&0x3ff)
#define JP_MASK(x)			(((x)>>10)&0x07)
// Two op masks
#define TA_DREG(x)          ((x)&0x0f)
#define TA_AS(x)			(((x)>>4)&0x03)
#define TA_BW(x)			(((x)>>6)&0x01)
#define TA_AD(x)			(((x)>>7)&0x01)
#define TA_SREG(x)			(((x)>>8)&0x0F)
#define TA_MASK(x)			(((x)>>12)&0x0F)

// Table of function pointers for one op instructions
vector< function< void(unsigned, unsigned, unsigned) > > oa_ptr{
	rrc, swpb, rra, sxt, push, call, reti };


// Table of function pointers for two op instructions
void(*ta_ptr[])(unsigned, unsigned, unsigned, unsigned, unsigned) = {
	ta_not_implemented, ta_not_implemented, ta_not_implemented,
	ta_not_implemented,	mov, add, addc, subc, sub, cmp, ta_not_implemented,
	bit, bic, bis, xor, and };

// Table of function pointers for jump instructions
void(*jp_ptr[])(unsigned) = {
	jne, jeq, jnc, jc, jn, jge, jl, jmp };

int machine() {
	uint16_t inst_reg = 0;
	// while within instruction memory range and HCF not set
	while (registers[PC] < registers[SP] && !HCF) {
		check_for_status_change();
		inst_reg = fetch();
		if (inst_reg != 0) {
			switch (OPCODE_TYPE(inst_reg)) {
			case 0:
				oa_ptr[OA_MASK(inst_reg)]
				(OA_BW(inst_reg), OA_AS(inst_reg), OA_REG(inst_reg));
				// if in debug mode, output diagnostic information to console
				#ifdef DEBUG
					cout << endl << endl;
					cout << "INST:      " << last_inst << endl;
					cout << "SRCOP:     " << bitset<16>(last_srcop) << endl;
					cout << "RESULT:    " << bitset<16>(last_result) << endl;
					cout << "{V,N,Z,C}: " << "{" << srptr->overflow << ", " 
					<< srptr->negative << ", " << srptr->zero << ", "
					<< srptr->carry << "}" << endl;
					cout << "SYSCLK:    " << dec << sys_clk << endl;
				#endif
				break;
			case 1:
				jp_ptr[JP_MASK(inst_reg)](JP_OFF(inst_reg));
				break;
			default:
				ta_ptr[TA_MASK(inst_reg)]
				(TA_BW(inst_reg), TA_AS(inst_reg), TA_SREG(inst_reg), TA_AD(inst_reg), TA_DREG(inst_reg));
				// if in debug mode, output diagnostic information to console
				#ifdef DEBUG
					cout << endl << endl;
					cout << "INST:      " << last_inst << endl;
					cout << "SRCOP:     " << bitset<16>(last_srcop) << endl;
					cout << "DSTOP:     " << bitset<16>(last_dstop) << endl;
					cout << "RESULT:    " << bitset<16>(last_result) << endl;
					cout << "{V,N,Z,C}: " << "{" << srptr->overflow << ", " 
					<< srptr->negative << ", " << srptr->zero << ", " 
					<< srptr->carry << "}" << endl;
					cout << "SYSCLK:    " << dec << sys_clk << endl;
				#endif
				break;
			}
		}
	}
	return MACHINE_DONE;
}

// fetch next instruction from memory over the bus 
uint16_t fetch() {
	uint16_t temp;							
	MBR_RD(registers[PC], temp, WORD);		
	INC_PC;									
	return temp;							
}

// grab a word from memory without using bus (no PC/SYSCLK increment)
uint16_t get_word(int loc) {
	uint16_t temp = (memory[loc + 1] << 8) | memory[loc];
	return temp;
}

// check if arithmetic operation resulted in carry out of the MSB
// accomplished by checking bit 16 of the 32 bit result obtained
// from the arithmetic operation of 16 bit operand(s)
void handler_carry(uint32_t &result, unsigned bw) {
	if (bw == BYTE) {
		srptr->carry = GET_BIT(result, 8);

	}
	else {
		if (GET_BIT(result, 16) == 1)
			srptr->carry = SET;
		else
			srptr->carry = CLR;
	}
}

// check if arithmetic operation resulted in negative result
// accomplished by checking bit 7 for byte or bit 15 for word
void handler_negative(uint32_t &result, unsigned bw) {
	if (bw == BYTE) {
		if (GET_BIT(result, 7) == 1)
			srptr->negative = SET;
		else
			srptr->negative = CLR;
	}
	else {
		if (GET_BIT(result, 15) == 1)
			srptr->negative = SET;
		else
			srptr->negative = CLR;
	}
}

// check if arithmetic operation resulted in overflow
// overflow is defined as a loss of sign information
// and happens when the addition of two operands with
// the same sign yields a result with the opposite sign
// ex: pos + pos = neg 
// ex: neg + neg = pos
// accomplished by comparing signs of operands and result
void handler_overflow(uint16_t &srcop, uint16_t &dstop, uint32_t &result, unsigned bw) {
	bool srcneg, dstneg, resneg;
	if (bw == BYTE) {
		srcneg = GET_BIT(srcop, 7);
		dstneg = GET_BIT(dstop, 7);
		resneg = GET_BIT(result, 7);
	}
	else {
		srcneg = GET_BIT(srcop, 15);
		dstneg = GET_BIT(dstop, 15);
		resneg = GET_BIT(result, 15);
	}
	if (srcneg == dstneg)
		if (srcneg != resneg)
			srptr->overflow = SET;
		else
			srptr->overflow = CLR;
	else
		srptr->overflow = CLR;
}

// check if result of arithmetic operation is zero
void handler_zero(uint32_t &result, unsigned bw) {
	if (bw == BYTE)
		result &= 0xFF;
	else
		result &= 0xFFFF;
	if (result == 0)
		srptr->zero = SET;
	else
		srptr->zero = CLR;
}