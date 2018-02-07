/****************************************************************************************
*	FILE: loader.cpp
*
*	ECED 3403 - Computer Architecture
*	Stephen Sampson - B00568374
*	Assignment #2 - MSP430 Emulator
*	Summer 2017
*
*				Last Modified	Author
*	ORIGINAL:	July 2017		Stephen Sampson
*
*	This file contains:
*		Program flow to read the s19 file line by line and load data into 
*		appropriate locations in memory.  The loader also checks for the 
*		existence of an s9 record and if found changes the starting execution 
*		address to the address specified.
****************************************************************************************/

#include "globals.h"
#include "loader.h"
#include "machine.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>					

// custom namespace for performing filesystem operations
namespace fs = std::experimental::filesystem;		
using namespace std;


/****************************************************************************************
*						   	      FUNCTION PROTOTYPES
****************************************************************************************/

int handles19(string &oneline);
int handle_s1(string &oneline);
int handle_s5(string &oneline);
int handle_s9(string &oneline);

/****************************************************************************************
*						   	      MAIN LOADER EXECUTION
****************************************************************************************/
int loader(string &fn) {	
	ifstream inFile;
	string oneline; 
	
	// if empty string passed to loader request filename to open
	if(fn.empty()) {						
		cout << "Please type the name of the s19 record you would like to load: ";
		cin >> fn;
	// otherwise extract filename from command line argument passed to loader
	} else {
		fn = fs::path(fn).filename().string();
	}
	
	cout << "ATTEMPTING TO OPEN: " << fn << endl;				// write to console
	inFile.open(fn);											// attempt to open filename
	if (!inFile.is_open())										// if filename is not open
		return LOAD_BADFILE;									// return bad file
	cout << "SUCCESSFULLY OPENED: " << fn << endl;				// write to console
	cout << "LOADING DATA TO MEMOERY... PLEASE WAIT" << endl;	// write to console
	while (inFile.peek() != EOF) {								// while we are not at the end of file
		getline(inFile, oneline);								// get next line (srecord)
		if (handles19(oneline) != LOAD_SUCCESS)					// if handles19 can not load the data
			return LOAD_BADRECORD;								// return bad srecord
	}
	cout << "FINISHED LOADING TO MEMORY" << endl;				// write to console
	return LOAD_SUCCESS;										// return load success 
}

// differentiates between s1, s5, and s9 records and calles the appropriate handler
int handles19(string &oneline) {
	if(oneline.at(0)!='S')										// verify formatting
		return LOAD_BADTYPE;
	switch (oneline[1]) {										// switch on second character
	case '1':
		return handle_s1(oneline);								// handle s1 record
	case '5':
		return handle_s5(oneline);								// handle s5 record
	case '9':
		return handle_s9(oneline);								// handle s9 record
	default:
		return LOAD_BADTYPE;									// second char != 1|5|9
	}
}

// loads data from s1 record into memory
int handle_s1(string &oneline) {
	stringstream ss;											// stingstream ss
	int len = oneline.length();									// lenth of srecord string
	uint16_t tempbyte;											// used to store each data byte
	string addrstr = oneline.substr(4, 4);						// separate address from s1 record
	string datastr = oneline.substr(8, (len - 10));				// separate data from s1 record
	unsigned address;											// used to store srecord address
	sscanf_s(addrstr.c_str(), "%4x", &address);					// store address srecord starts at in &address
	for (unsigned i = 0; i < datastr.length() / 2; i++) {		// two characters represent each byte so we go to length / 2
		ss << datastr.substr(i * 2, 2);							// read next two characters into string stream
		ss >> hex >> tempbyte;									// store these in tempbyte 
		memory[address] = tempbyte;								// load data into memory
		address++;												// increment address
		ss.clear();												// clear stringstream
	}
	return LOAD_SUCCESS;										// return to calling function 
}

// not needed for this assignment - here for future versions of project
int handle_s5(string &oneline) {
	return LOAD_SUCCESS;
}

// sets starting execution address based on address specified in s9 record
int handle_s9(string &oneline) {								
	stringstream ss;											// stringstream ss
	int address = 0;											// used to store srecord address
	string addrstr = oneline.substr(4, 4);						// separate address from s9 record
	ss << addrstr;												// read address string into stringstream
	ss >> hex >> address;										// store stringstream contents as hex into address
	registers[PC] = address;									// set program counter to value specified in s9 record
	start_addr = address;										// set global var 'start_addr'
	return LOAD_SUCCESS;										// return to calling function
}