#include "miscFunctions.h"

DWORD stringToDword(const char* input) {

	DWORD output = 0;

	for (int i = 0; input[i] != 0; i++) {
		output <<= 8;
		output |= input[i];
	}

	return output;

}

in_addr getAddr(const char* server) {
		
	in_addr addr;
	addr.s_addr = 0;
	
	
	
	//check to see if they gave us an IP address
	unsigned long temp = inet_addr(server);
	
	if (temp != INADDR_NONE) {
		addr.s_addr = temp;
		return addr;
	}

	
	
	//they didn't give us an IP address, query the host
	hostent* host;
	host = gethostbyname(server);
	
	if (host == 0)
		return addr;
	
	
	memcpy(&addr, host->h_addr_list[0], sizeof(addr));
		

	return addr;
	
}

string getConfigString(string section, string key, string filename, string def) {

	string filepath = currentDir() + "\\" + filename;

	char buf[1024];
	GetPrivateProfileString(section.c_str(), key.c_str(), def.c_str(), buf, sizeof(buf), filepath.c_str());

	string ret = buf;
	return ret;

}

int getConfigInt(string section, string key, string filename, int def) {

	string filepath = currentDir() + "\\" + filename;

	int ret = GetPrivateProfileInt(section.c_str(), key.c_str(), def, filepath.c_str());
	return ret;

}

string currentDir() {

	char path[MAX_PATH];
	GetCurrentDirectory(sizeof(path), path);

	string ret = path;
	return ret;

}