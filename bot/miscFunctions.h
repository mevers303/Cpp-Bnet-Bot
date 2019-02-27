#ifndef _MISCFUNCTIONS_H
#define _MISCFUNCTIONS_H

#include "includes.h"
using namespace std;

//converts char array to dword, basically just reverses it and removes the null terminator
//you can use this on platform or product ids, i have no clue what else you'd use it for
DWORD stringToDword(const char* input);

//gets the in_addr for a server
//the server string can be an ip or hostname
in_addr getAddr(const char* server);

//wrapper for getprivateprofilestring
string getConfigString(string section, string key, string filename = "config.ini", string def = "");

int getConfigInt(string section, string key, string filename = "config.ini", int def = 0);

string currentDir();

#endif