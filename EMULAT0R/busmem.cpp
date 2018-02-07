/****************************************************************************************
*	FILE: busmem.cpp
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
*	This file contains bus and memory emulation
*		- Bus controls access to memory
*		- Memory is 64kib
*		- Allows byte or word (odd or even byte) reads
*		- No indication of location is data or inst
*		- Devices can directly access memory (low mem)
****************************************************************************************/

#include "globals.h"
#include "machine.h"
#include <bitset>

using namespace std;

void bus(unsigned short address, unsigned short *data, unsigned rw, unsigned bw) {
// This function emulates the bus:
// All address references refer to a byte
// Words: little endian format (addr+1 << 8 | addr)
// READ: return location contents (BYTE or WORD)
// WRITE: write to specified location (BYTE or WORD)
// All low-level memory accesses that affect devices -- call device emulator
// Device emulator is called first to avoid repairing overwritten device memory

	if (address < DEVICE_LIMIT) {
		/* Device memory access
		- Byte access by defautl
		- cast 'data' (short ptr) to char ptr
		*/
		dev_mem_access(address, reinterpret_cast<unsigned char *>(data), rw);
//		sys_clk += MEM_ACCESS_TIME;
	} else {
		/* Accessing any non-device memory locations */
		if (rw == READ) {
			if (bw == BYTE) {
				*data = memory[address];
			} else {
				*data = (memory[address + 1] << 8) | memory[address];
			}
		} else { /* rw = WRITE */
			memory[address] = (*data) & 0xff; /* low byte always copied */
			if (bw == WORD)
				memory[address + 1] = (*data >> 8) & 0xff; /* high byte */
		}
		sys_clk ++;		// bus access takes one clock cycle
	}
}

