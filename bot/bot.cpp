#include "bot.h"

void basicBot::loadConfig() {

	username = getConfigString("username");
	password = getConfigString("password");
	cdKey = getConfigString("cdKey");
	product = getConfigString("product");

}

basicBot::basicBot(string profile) {

	profileName = profile;

	errorOccurred = false;

	initializeSocket();

	loadConfig();

}

basicBot::~basicBot() {
	
}

//initializes winsock and creates the socket
bool basicBot::initializeWinsock() {
	
	cout << "Initializing Winsock..." << endl;


	WSAData wsaData;
	

	int result = WSAStartup(0x0101, &wsaData);
	
	if (result != NO_ERROR) {
		cout << "Winsock could not be initialized.  The bot(s) will not be able to connect." << endl;
		return false;
	}

	cout << "Winsock initialized!" << endl;
	return true;

}

bool basicBot::initializeSocket() {
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		error("The socket could not be created.  This bot will not be able to connect.");
		return false;
	}

	return true;

}

//closes the socket and cleans up winsock
void basicBot::cleanupWinsock() {
	
	WSACleanup();

}

//connects to the specified server and port
bool basicBot::connect(string server, unsigned short port) {

	cout << profileName << ": Connecting to " << server << "..." << endl;

	sockaddr_in sin;
	in_addr addr;
	
	

	addr = getAddr(server.c_str());
	
	sin.sin_family = AF_INET;
	sin.sin_addr = addr;
	sin.sin_port = htons(port);
	
	
	
	int result = ::connect(sock, (SOCKADDR*)&sin, sizeof(sin));
	
	if (result == SOCKET_ERROR) {
		error("Could not connect to the server.");
		return false;
	}


	return true;
	
}

void basicBot::disconnect() {
	closesocket(sock);
	cout << "<-> Disconnected!" << endl;
}

void basicBot::error(string message) {
	cout << message << endl;
	disconnect();
	errorOccurred = true;
}

void basicBot::botMain() {
	
	if (errorOccurred) {
		cout << "An error occurred while the bot was starting up, aborting botMain..." << endl;
		return;
	}


	connect(getConfigString("server"));

	

	sendProtocol();
	send0x50();


	char* buf = new char[0];
	int bufLen = 0;

	static char dummyBuf[5120];

	while ((bufLen = recv(sock, dummyBuf, 5120, MSG_PEEK)) > 0) {

		delete[] buf;
		buf = new char[bufLen];

        recv(sock, buf, bufLen, 0);


		int currentPos = 0;

		while (bufLen - currentPos > 3) {

            WORD bnetPacketLen = *(WORD*)(buf + currentPos + 2);

			parsePacket(buf + currentPos, bnetPacketLen);
			
			currentPos += bnetPacketLen;

		}

    }

    if (bufLen == 0)
		error("Battle.net has closed the connection!");
	else if (bufLen < 0 && (! errorOccurred)) {
		error("A socket error occurred while trying to receive data.");
		cout << "The error code is " << WSAGetLastError() << "." << endl;
	}

}

string basicBot::getConfigString(string key, string def) {

	return ::getConfigString(profileName.c_str(), key, "profiles.ini", def);

}

int basicBot::getConfigInt(string key, int def) {

	return ::getConfigInt(profileName.c_str(), key, "profiles.ini", def);

}
