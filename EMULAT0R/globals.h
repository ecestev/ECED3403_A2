/******************************************************************************
Global Header File
ECED 3403 - Computer Architecture
Stephen Sampson - B00568374
July 2017
******************************************************************************/

#pragma once
#include <iomanip>

/* Global Variables */
extern bool HCF;			// flag indicating whether emulation should continue
extern int start_addr;

/* Global Structures */
extern struct sr_bits *srptr;		// pointer to CPU SR bits
extern uint8_t memory[];			// array of bytes representing primary memory
extern uint16_t registers[];		// array of words representing CPU registers
extern unsigned long sys_clk;		// stores clock cyle count
extern unsigned long max_clk;

/* Global Vars / Streams for Devices */
extern std::ifstream devices_in;
extern std::ofstream devices_out;
extern std::string deviceline;
#define DEVICE_FILE "devices.in"
#define DEVICE_OFILE "devices.out"

/* Global Macros  */
#define NUL = '\0'
#define HEX2( x ) uppercase << setw(2) << setfill('0') << hex << (int)(x)
#define HEX4( x ) uppercase << setw(4) << setfill('0') << hex << (int)(x)


#define DEBUG

#ifdef DEBUG
	/* GLOBAL DIAGNOSTIC VARS */
	extern std::string last_inst;		// DIAG OUTPUT IF DEBUG MODE
	extern uint16_t last_srcop;			// DIAG OUTPUT IF DEBUG MODE
	extern uint16_t last_dstop;			// DIAG OUTPUT IF DEBUG MODE
	extern uint16_t last_result;		// DIAG OUTPUT IF DEBUG MODE
#endif