/******************************************************************************
Loader Header File
ECED 3403 - Computer Architecture
Stephen Sampson - B00568374
July 2017
******************************************************************************/

#pragma once
int loader(std::string &fn);
enum LOADER_RTN_CODES { LOAD_SUCCESS, LOAD_BADFILE, LOAD_BADRECORD, LOAD_BADTYPE, LOAD_CHKSUM };