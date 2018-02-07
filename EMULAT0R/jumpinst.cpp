/****************************************************************************************
*	FILE: jumpinst.cpp
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
*	This file contains Jump instructions
*		- Common code: 
*			Verify jump condition (if conditional jump) 
*			Extract offset and convert to signed 16-bits
*			Change program counter if jump condition is met
****************************************************************************************/

#include "globals.h"
#include "machine.h"
#include <iostream>

#define OFFMASK      0x03FF
#define SIGNEXT      0xFC00
#define SIGN10       0x0200

using namespace std;

signed short offset10to16(unsigned short off10) {
	/* Convert 10-bit signed to 16-bit signed */
	if (off10 & SIGN10)
		/* Negative */
		return (SIGNEXT | (off10 & OFFMASK)) << 1;
	/* Positive */
	return (off10 & OFFMASK) << 1;
}

void jmp_not_implemented(unsigned offset) {
	cout << "Jump instruction not implemented PC: " <<  registers[PC] << endl;
}

void jng(unsigned offset) {
	// Jump if negative
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (srptr->negative) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JNG";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jne(unsigned offset) {
	// Jump if not equal | Jump if not zero
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (!srptr->zero) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JNE";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jeq(unsigned offset) {
	// Jump if equal | Jump if zero
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (srptr->zero) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JEQ";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jnc(unsigned offset) {
	// Jump if carry not set
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (!srptr->carry) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JNC";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jc(unsigned offset) {
	// Jump if carry set
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (srptr->carry) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	}  
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JC";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jn(unsigned offset) {
	// Jump if negative
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (srptr->negative) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JN";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jge(unsigned offset) {
	// Jump if greater or equal; (N .XOR. V) = 0
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if (!(srptr->negative ^ srptr->overflow)) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	}
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JGE";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jl(unsigned offset) {
	// Jump if less; (N.XOR.V) = 1
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	if ((srptr->negative ^ srptr->overflow)) {
		registers[PC] += offset10to16(offset);
		#ifdef DEBUG
			last_dstop = registers[PC] + offset10to16(offset);	// DIAG OUTPUT IF DEBUG MODE
		#endif
	} 
	#ifdef DEBUG
		else
			last_dstop = registers[PC];							// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JL";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}

void jmp(unsigned offset) {
	//	Unconditional Jump
	#ifdef DEBUG
		last_srcop = registers[PC];								// DIAG OUTPUT IF DEBUG MODE
	#endif
	registers[PC] += offset10to16(offset);
	#ifdef DEBUG
		last_dstop = registers[PC] + offset10to16(offset);		// DIAG OUTPUT IF DEBUG MODE
		last_inst = "JMP";										// DIAG OUTPUT IF DEBUG MODE
	#endif
}
