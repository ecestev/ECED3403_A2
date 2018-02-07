/****************************************************************************************
*	FILE: addrmodes.cpp
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
*	This file contains:
*		Program flow to retrieve operands from and store operands to primary memory.
*		The location of the operand is determined by the addressing mode which is 
*		identified by interpreting the the AS/AD. Operand is then read from / written
*		to memory over the bus
****************************************************************************************/

#include "globals.h"
#include "machine.h"
#include <iostream>

using namespace std;

/*
- Source addressing mode - map As and Reg to mode (1..7)
- Special cases: constant generators 0xC#, #=0, 1, 2, 4, 8, F (-1)

- Destination address mode - same as source but using Ad not As
- Ad (0|1) allows modes 1..4
*/

enum addr_modes {
	REG = 1, INDX = 2, REL = 3, ABS = 4,
	IND = 5, INDI = 6, IMM = 7
};

/*
Source-destination addressing mode matrix
[4] - Four As combinations (00..11) and two Ad combinations (0..1)
[16] - 16 registers (R0..R15 or 0000..1111]

sd_am[asd][regnum] - addressing mode

Constants:
Special cases - depend on As value (00..11) and regnum (SR/R2 or CG/R3)
Constants encoded as 0xC# where '#' is 0, 1, 2, 4, 8, F
Decoded in get_operand()
*/

PRIVATE char sd_am[4][16] = {
	/*00*/{ REG,  REG,  REG,  0xC0, REG,  REG,  REG,  REG,  REG,  REG,  REG,  REG,  REG,  REG,  REG,  REG },  
	/*01*/{ REL,  INDX, ABS,  0xC1, INDX, INDX, INDX, INDX, INDX, INDX, INDX, INDX, INDX, INDX, INDX, INDX },  
	/*10*/{ IND,  IND,  0xC4, 0xC2, IND,  IND,  IND,  IND,  IND,  IND,  IND,  IND,  IND,  IND,  IND,  IND },  
	/*11*/{ IMM,  INDI, 0xC8, 0xCF, INDI, INDI, INDI, INDI, INDI, INDI, INDI, INDI, INDI, INDI, INDI, INDI }
};

unsigned short get_operand(unsigned asd, unsigned regnum) {
	/*
	- Extract operand (word) according to asd (As or Ad) and regnum
	- Caller must decide whether byte or word operation
	- incr_pc signals whether the PC should be incremented:
	* one-address INDX 'X' - do not incr (incr on put_opr())
	* one-address REL - do not incr (incr on put_opr())
	*/
	unsigned short x;			// represents offset for addressing modes
	unsigned short mode ;		// Addr mode obtained from sd_am[][]
	unsigned short operand;		// the operand to be used executing the instruction
	
	switch ((mode = sd_am[asd][regnum])) {
		
		case REG:	// EA = Rn
			operand = registers[regnum];
			break;
		case INDX:	// EA = Rn + [PC]	
			MBR_RD(registers[PC], x, WORD);
			MBR_RD( ( registers[ regnum ] + x ), operand, WORD);
			break;
		case REL:	// EA = PC + [PC]
			MBR_RD(registers[PC], x, WORD);
			MBR_RD((registers[PC] + x), operand, WORD);
			break;
		case ABS:	// EA = [PC]
			MBR_RD(registers[PC], x, WORD);
			MBR_RD(x, operand, WORD);
			INC_PC;
			break;
		case IND:	// EA = [Rn]		
			MBR_RD(registers[regnum], operand, WORD);
			break;
		case INDI:	// EA = [Rn]
			MBR_RD(registers[regnum], operand, WORD);
			break;
		case IMM:	// EA = PC			
			MBR_RD(registers[PC], operand, WORD);
			INC_PC;
			break;
		default:
			operand = mode & 0x0f;
			if (operand == 0x0f)
				operand = -1;
			break;
	}



	return operand;
}

void put_operand(unsigned asd, unsigned regnum, unsigned short operand) {
	/*
	- Put operand into location specified by asd and regnum
	- One-operand instruction - can use all As combinations (including constants)
	- Two-operand instruction - can only use Ad combinations only
	- As/Ad check done by caller
	*/

	unsigned short mode;		// Addr mode obtained from sd_am[][]
	unsigned short x;			// represents offset for addressing modes

	switch ((mode = sd_am[asd][regnum])) {
		case REG:
			registers[regnum] = operand;
			break;
		case INDX:
			MBR_RD(registers[PC], x, WORD);
			MBR_WR((x + registers[regnum]), operand, WORD);
			INC_PC;
			break;
		case REL:
			MBR_RD(registers[PC], x, WORD);
			MBR_WR((registers[PC] + x), operand, WORD);
			INC_PC;
		case ABS:
			MBR_RD(registers[PC]-2, x, WORD);
			MBR_WR(x, operand, WORD);
			break;
		case IND:
			MBR_WR(registers[regnum], operand, WORD);
			break;
		case INDI:
			MBR_WR(registers[regnum], operand, WORD);
			registers[regnum] += 2;
			break;
		case IMM:
			MBR_WR(registers[PC]-2, operand, WORD);
			break;
		default:
			/* Constants - invalid operand for Ad */
			/* convert encoding to binary */
			operand = mode & 0x0f;
			if (operand == 0x0f)
				operand = -1;
			break;
	}
}
