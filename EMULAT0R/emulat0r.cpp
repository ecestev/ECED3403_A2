/****************************************************************************************
*	FILE: emulat0r.cpp
*	
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*	
*				Last Modified	Author
*	ORIGINAL:	July 2017		Stephen Sampson
*
*	This file contains the main program flow for the
*	Texas Instrument's MSP-430 microcomputer emulator
****************************************************************************************/

#include <windows.h>									
#include <iostream>										
#include <fstream>										
#include <signal.h>
#include "debugger.h"									
#include "loader.h"										
#include "machine.h"									 
#include "globals.h"									

using namespace std;

/****************************************************************************************
*							  INITIALIZE GLOBAL VARS
****************************************************************************************/

#ifdef DEBUG
	string last_inst;									// FOR DIAGNOSTIC OUTPUT
	uint16_t last_srcop;								// FOR DIAGNOSTIC OUTPUT
	uint16_t last_dstop;								// FOR DIAGNOSTIC OUTPUT
	uint16_t last_result;								// FOR DIAGNOSTIC OUTPUT
#endif

bool HCF = false;										// flag to continue/halt emulation
int start_addr = 0;										// address to start execution - set by loader

uint8_t memory[65535] = {};								// primary memory array (0 initialized)

uint16_t registers[16] = {0,0xFFC0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned long sys_clk = 0;								// contains present value of system clock
unsigned long max_clk = 0;								// use to specify how many clock cycles to run for

struct sr_bits *srptr =									// bit accessible pointer to 
	reinterpret_cast<struct sr_bits *>(&registers[SR]);	// status register

enum emulator_return_codes {EMULATOR_SUCCESS, EMULATOR_FAILURE};

int main(int argc, char *argv[]) {
	/************************************************************************************
	*                             DECLARATIONS & INITIALIZATIONS
	************************************************************************************/
	cout << "------------------------------------------------------------" << endl;;
	cout << "                    MSP-430 ISA EMULATOR                " << endl;
	cout << "                Written By: Stephen Sampson			 " << endl;
	cout << "                    Dalhousie University				 " << endl;
	cout << "         Dept. Electrical and Computer Engineering      " << endl;
	cout << "------------------------------------------------------------" << endl << endl;
	cout << "NOTE TO USER:" << endl;
	cout << "     - To initiate the debugger, simultaneously press both" << endl;
	cout << "       'CTRL' and 'C' on your keyboard at any time." << endl;
	#ifdef DEBUG
		cout << "     - Emulation will run with full diagnostic information" << endl << endl;
	#endif
	#ifndef DEBUG
		cout << "     - Emulation will run with limited diagnostic information" << endl << endl;
	#endif
	signal(SIGINT, debugger);							// call debugger on signal
	string filename;									// variable to store filename 
	if(argc==2)											// if there are two command line args
		filename = argv[1];								// set filename based on first arg
	if (loader(filename) == LOAD_SUCCESS) { 			// if loader is able to sucessfully load 'filename'
		init_devices();									// initiazlie devices
		cout << "MSP-430 ISA EMULATION STARTED AT MEMORY[0x" << HEX4(registers[PC]) << "]" << endl;
		machine();										// begin emulation
	}
	else												// otherwise if loader fails
		cout << "ERROR LOADING FILE" << endl;			// let user know there was an error loading 'filename'
	while (!HCF)										// while flag 'run_machine' is true
		;												// do nothing
	cout << "Emulation Stopped... Exiting" << endl;		// 'run_machine' has been set to false
	Sleep(1000);										// sleep to display diagnostic message to user
	return EMULATOR_SUCCESS;							// exit
	/************************************************************************************
	*                                 END OF PROGRAM
	************************************************************************************/
}