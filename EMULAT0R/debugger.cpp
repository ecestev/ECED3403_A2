/****************************************************************************************
*	FILE: debugger.cpp
*
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*	
				Last Modified	Author
*	ORIGINAL:	July 2017		Stephen Sampson
*
*	This file contains: A very simple debugger. Suuprted commands are:
*		S: Stop Emulation 
*			- sets HCF = true
*		M: Print memory range to console
*			- user specifies starting and ending hex address to print
*		R: Print Registers to console
*		P: Print Registers and all non-zero memory to console
*		L: Load new s19
*			- allows user to specify new srecord file to load and calls loader
*		X: Do nothing. Exit Debugger.*	
****************************************************************************************/

#include "globals.h"
#include "debugger.h"
#include "loader.h"
#include "machine.h"
#include <iostream>
#include <signal.h>

using namespace std;

void debugger(int param) {
	string news19 = {};
	uint16_t start = 0;
	uint16_t end = 0;
	char input = NULL;
	cout << endl;
	cout << "------------------------------------------------------------" << endl;;
	cout << "                   DEBUGGER INITIATED..." << endl;
	cout << "------------------------------------------------------------" << endl << endl;
	cout << "This is a simple debugger. Supported commands:" << endl;
	cout << "     1) S: Stop Emulation" << endl;
	cout << "     2) M: Print Memory Range to Console" << endl;
	cout << "     3) R: Print Registers to Console" << endl;
	cout << "     4) P: Print Registers and all Non-Zero Memory to Console" << endl;
	cout << "     5) L: Load new s19" << endl;
	cout << "     6) X: Do nothing. Return" << endl << endl;
	cout << "ENTER COMMAND: ";
	cin >> input;
	input = toupper(input);
	switch (input) {
	case 'S':	cout << "STOPPING MACHINE" << endl;
		HCF = true;
		break;

	case 'M':
		cout << "Please enter the starting address (hex) to output: ";
		cin >> hex >> start;
		cout  << "Please enter the last address (hex) you would like to output: ";
		cin >> hex >> end;
		cout << "Printing Memory Range to Console:" << endl;
		for(int i = start; i < end; i+=2) {
			uint8_t LO_BYTE = memory[i];
			uint8_t HI_BYTE = memory[i+1];
			uint16_t temp = HI_BYTE;
			temp <<= 8;
			temp += LO_BYTE;
			cout << "Location 0x" << HEX4(i) << ": " << HEX4(temp) << endl;
		}
		break;

	case 'R':
		cout << "Printing Register Contents to console:" << endl;
		for (int i = 0; i < 16; i ++) {
			cout << "R" << dec << i << ": 0x" << HEX4(registers[i]) << endl;
		}
		break;
	
	case 'P':
		cout << endl << endl;
		cout << "REGISTER CONTENTS; Loc: R0 - R15" << endl;
		for (int i = 0; i < 16; i++)
			cout << "R" << dec << i << ": 0x" << HEX4(registers[i]) << endl;

		cout << endl << endl;
		cout << "DATA MEMORY; Loc: 0x0000 - 0x" << HEX4(start_addr) << endl;
		for (int i = 0; i < start_addr; i += 2) {
			uint8_t LO_BYTE = memory[i];
			uint8_t HI_BYTE = memory[i + 1];
			uint16_t temp = HI_BYTE;
			temp <<= 8;
			temp += LO_BYTE;
			if (temp != 0)
				cout << "Location 0x" << HEX4(i) << ": 0x" << HEX4(temp) << endl;
		}

		cout << endl << endl;
		cout << "INSTRUCTION MEMORY; Loc: 0x" << HEX4(start_addr) << " - SP(0x" << HEX4(registers[SP]) << ")" << endl;
		for (int i = start_addr; i < registers[SP]; i += 2) {
			uint8_t LO_BYTE = memory[i];
			uint8_t HI_BYTE = memory[i + 1];
			uint16_t temp = HI_BYTE;
			temp <<= 8;
			temp += LO_BYTE;
			if (temp != 0)
				cout << "Location 0x" << HEX4(i) << ": 0x" << HEX4(temp) << endl;
		}

		cout << endl << endl;
		cout << "STACK MEMORY; Loc: 0x" << HEX4(registers[SP]) << " - 0xFFC0" << endl;
		for (int i = registers[SP]; i < TOS; i += 2) {
			uint8_t LO_BYTE = memory[i];
			uint8_t HI_BYTE = memory[i + 1];
			uint16_t temp = HI_BYTE;
			temp <<= 8;
			temp += LO_BYTE;
			if (temp != 0)
				cout << "Location 0x" << HEX4(i) << ": 0x" << HEX4(temp) << endl;
		}

		cout << endl << endl;
		cout << "HIGH MEMORY (ISRVECTS); Loc: 0xFFC0 - 0xFFFF" << endl;
		for (int i = TOS; i < 65535; i += 2) {
			uint8_t LO_BYTE = memory[i];
			uint8_t HI_BYTE = memory[i + 1];
			uint16_t temp = HI_BYTE;
			temp <<= 8;
			temp += LO_BYTE;
			if (temp != 0)
				cout << "Location 0x" << HEX4(i) << ": 0x" << HEX4(temp) << endl;
		}
		break;

	case 'L':
		cout << "Type the name of the .s19 file to load: " << endl;
		cin >> news19;
		for (int i = 0; i < 65535; i++)
			memory[i] = 0;
		if (loader(news19) == LOAD_SUCCESS) 				
			machine();										
		else												
			cout << "ERROR LOADING FILE" << endl;			
		break;

	case 'X':
		cout << "Closing Debugger" << endl;
		break;

	default:
		;			// do nothing

	}
	signal(SIGINT, debugger);
}
