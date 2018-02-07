/****************************************************************************************
*	FILE: devices.cpp
*
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*
*				Last Modified	Author
*	ORIGINAL:	June 2017		Dr. Hughes
*	MODIFIED:	July 2017		Stephen Sampson
*
*	This file contains code to handle devices:
*		- Emulates up to 16 devices
*		- All have same register structure:
*			n: Reserved [4], OF [1], DBA [1], IO [1], IE [1]
*			n+1: Data (I/O)
*		- I/O controlled by input file
*		- Caveat emptor
*
****************************************************************************************/

/*

ECED 3403
17 06 14 - Fix cause_interrupt() to change SP correctly
16 06 22 - Changes to bus() - order of memory access
16 06 16 - Modifications to cause_interrupt()
15 06 09 - Initial implementation
*/

#include <windows.h>	
#include "machine.h"
#include "globals.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <bitset>


using namespace std;

/* Status-control register overlay - common to all devices */
struct scr_olay {
	unsigned ie : 1;   /* Interrupt enable RW */
	unsigned io : 1;   /* Input or Output device - emulator selected RO */
	unsigned dba : 1;  /* Data Byte/Buffer Available - emulator RO */
	unsigned of : 1;   /* Overflow - emulator RO */
	unsigned res : 4;  /* Reserved - RO */
};

/* Device properties - common to all devices (note write-specific fields) */
struct device_properties {
	unsigned in_out : 1;        /* 0 - output | 1 - input */
	unsigned output_active : 1; /* T|F - outputting byte */
	unsigned char in_out_data;  /* byte being transmitted */
	unsigned output_time;       /* Constant - time for byte to xmit */
	unsigned end_time;          /* time proceessing is to finish */
};

/* Data specific to the devices - can only be accessed by device emulator */
PRIVATE struct device_properties devices[NUM_DEVICES];
/* Pending data for next device - from emulator input file */
PRIVATE uint16_t next_dev = 0;
PRIVATE char next_data = ' ';
PRIVATE unsigned long next_time = 0;
PRIVATE unsigned long last_time = 0; 
PRIVATE bool processed = false;

void init_devices() {
	unsigned in_out = 0, output_time = 0; /* From emulator input file record */
	unsigned dev = 0;
	/***
	- OPEN device status change file (device configuration and device status data)
	- OPEN device output file (diagnostics and device output)
	***/

	/***
	- READ input file - first 16 records for input/output and processing time
	***/
	devices_in.open(DEVICE_FILE);
	devices_out.open(DEVICE_OFILE);
	devices_out << "-----------------------------------" << endl;
	devices_out << " PROCESSING INTERRUPTS" << endl;
	devices_out << "-----------------------------------" << endl;
	for (dev = 0; dev < NUM_DEVICES; dev++) {
		/***
		- READ in_out and output_time (0 for input; !0 for output)
		***/
		getline(devices_in, deviceline);
		istringstream(deviceline) >> in_out >> output_time;
		devices[dev].in_out = in_out;
		devices[dev].output_time = output_time;
		devices[dev].output_active = FALSE;
		/* Application program must set/clear interrupts (mem[].ie) */
	}
	/***
	- READ input file for first status change - next_time, next_dev, next_data
	***/
	getline(devices_in, deviceline);
	istringstream(deviceline) >> next_time >> next_dev >> next_data;
	devices[next_dev].in_out_data = next_data;
	processed= false;
}

void cause_interrupt(unsigned device) {
	/*
	- Generate an interrupt:
	- TOS is in use, move SP up to next free word on stack
	- Push PC
	- Push SR (SP remaining pointing here)
	- New PC taken from device's interrupt vector
	- Compare with RETI (oneaddrinst)
	*/
	/* Push PC and SR - call bus() */
	registers[SP] -= 2;
	MBR_WR(registers[SP], registers[PC], WORD); /* SP points to empty TOS location */
	registers[SP] -= 2;   /* Next empty TOS location */
	MBR_WR(registers[SP], registers[SR], WORD);
	/* Clear SR - disable interrupts */
	registers[SR] = 0;
	/* Change PC to next_dev's vector */
	cout << "ISR_VECT_LOC: " << HEX4(VECTOR_BASE + (device * 2)) << endl;
	cout << "ISR_ENTRY_PT: " << HEX4(get_word(VECTOR_BASE + (device * 2))) << endl;
	cout << "PC CHANGED FROM: 0x" << HEX4(registers[PC]);
	registers[PC] = get_word(VECTOR_BASE + (device * 2));
	cout << " TO: 0x" << HEX4(registers[PC]) << endl;
	cout << "-----------------------------------" << endl;
	//	MBR_RD(VECTOR_BASE + (device * 2), registers[PC], WORD);
}

void check_for_status_change() {
	/*
	- Called after each instruction has been executed
	- Next_dev - pending device (from emulator input file)
	- Next_time - time of data arrival
	- Next_data - data for device at specified time
	- Check for input device status change
	* Generate interrupt if pending
	- Check for end of output devices' output processing
	* write completed - interrupt if necessary
	*/
	struct scr_olay *scr;


	/* Pending input device? */
	if (sys_clk >= next_time && processed == false) {
		/* Get status register */
		scr = reinterpret_cast<struct scr_olay *>(&memory[next_dev * 2]);

		/* Check for overflow (unread data register) */
		if (scr->dba == 1)
			scr->of = 1;
		/*
		- Next_data is (new) input byte
		- Signal data available (DBA)
		- Store in data register
		*/
		scr->dba = 1;
		/* Are device and CPU interrupts enabled? */
		if (scr->ie == 1 && srptr->gie == 1){
			devices_out << setw(10) << setfill(' ') << "INPUT_INT";
			devices_out << setw(13) << setfill(' ') << dec << "SYS_CLK: " << sys_clk;
			devices_out << setw(17) << setfill(' ') << "MEMORY_LOC: 0x" << HEX2((next_dev * 2 + 1));
			devices_out << setw(17) << setfill(' ') << "CHANGED FROM: 0x" << HEX2(memory[next_dev * 2 + 1]);
			memory[next_dev * 2 + 1] = next_data;
			devices_out << setw(7) << setfill(' ') << "TO: 0x" << HEX2(memory[next_dev * 2 + 1]);
			devices_out << setw(20) << setfill(' ') << "READ FROM DEV: " << next_dev << endl;
			processed = true;
			cout << endl;
			cout << "-----------------------------------" << endl;
			cout << "INTERRUPT DETECTED!" << endl;
			cout << "-----------------------------------" << endl;
			cout << "SYS_CLK: " << sys_clk << endl;
			cout << "0x" << HEX2(devices[next_dev].in_out_data) << " READ FROM DEVICE: " << next_dev << endl;
			cause_interrupt(next_dev);
			/***
			- READ input file for next status - next_time, next_dev, next_data
			***/
			if (devices_in.peek() != EOF) {
				getline(devices_in, deviceline);
				istringstream(deviceline) >> next_time >> next_dev >> next_data;
				devices[next_dev].in_out_data = next_data;
				processed = false;
			} 
			else {
				devices_out << "-----------------------------------" << endl;
				devices_out << " ALL INTERRUPTS PROCESSED" << endl;
				devices_out << "-----------------------------------" << endl;
				scr->ie = CLR;		// disable global interrupts - no more interrupts
			}
		}
	}


	/* Output device(s) finished writing? (Slow and dirty solution) */
	for (unsigned dev = 0; dev<NUM_DEVICES; dev++) {
		/* Check for output active device and time expired */
		if (devices[dev].output_active && sys_clk >= devices[dev].end_time) {
		
			/* Get status register */
			scr = reinterpret_cast<struct scr_olay *>(&memory[dev * 2]);
						
			/* WRITE finished - data buffer now available */
			scr->dba = 0;
			devices[dev].output_active = FALSE;

			/***
			- WRITE to output file: sys_clk, dev, output_data
			***/

			/* Are device and CPU interrupts enabled? */			
			if (scr->ie == 1 && srptr->gie == 1) {
				cout << endl;
				cout << "-----------------------------------" << endl;
				cout << "INTERRUPT DETECTED!" << endl;
				cout << "-----------------------------------" << endl;
				cout << "SYS_CLK: " << sys_clk << endl;
				cout << "0x" << HEX2(devices[dev].in_out_data) << " WRITTEN TO DEVICE: " << dev << endl;
				cause_interrupt(dev);
				devices_out << setw(11) << setfill(' ') << "OUTPUT_INT"
					<< setw(12) << setfill(' ') << dec << "SYS_CLK: " << sys_clk
					<< setw(18) << setfill(' ') << "OUT_DATA:   0x" << HEX2(devices[dev].in_out_data)
					<< setw(49) << setfill(' ') << dec << "WRITTEN TO DEV: " << dev << endl;
				scr->ie = CLR;
			}
		}
	}
}

void dev_mem_access(unsigned short address, unsigned char *data, unsigned rw) {
	/* Determine which device and its status register overlay */
	unsigned dev = address / 2;  /* 0:1, 2:3, .. 30:31 -> 0, 1, .. 15 */
	unsigned scr_addr = dev * 2; /* SCR address for this device */
	struct scr_olay *scr = reinterpret_cast<struct scr_olay *>(&memory[scr_addr]);

	/* Access status-control register */
	if (scr_addr == address) {
		if (rw == READ)
			*data = memory[scr_addr]; /* Return contents of SCR */
		else
			/* Attempt to write to SCR - only "ie" is RW */
			scr->ie = *data & 0x01; /* IE bit - others ignored */
		return;
	}
	/*
	Address is data register
	- Read from input dr: clear OF and DBA
	- Write to output dr: check DBA for OF, start output
	*/
	if (devices[dev].in_out == 1) {
		/* Input device */
		if (rw == READ) {
			/* READ - clear SCR bits and return data */
			scr->dba = 0;
			scr->of = 0;
			*data = memory[address];
		}
		/* WRITE is ignored -- can't write to input data register */
	} else{  /* Output device in_out == 0 */	
		if (rw == WRITE) {
			/* Check for pending data */
			if (scr->dba == 1)
				scr->of = 1;   /* Overflow - lose current byte */
			scr->dba = 1; /* Write active */
			memory[address] = *data;
			/* Save device state information */
			devices[dev].output_active = TRUE;
//			cout << endl << endl << "DEVICE: " << dev << " ACTIVE: = " << devices[dev].output_active  << endl;
			devices[dev].in_out_data = *data;
//			cout << "DEVICE: " << dev << " OUT DATA: = " << devices[dev].in_out_data << endl;
			devices[dev].end_time = sys_clk + devices[dev].output_time;
//			cout <<  "DEVICE: " << dev << " END TIME: = " << dec << devices[dev].end_time << endl << endl;
		}
	}
}
