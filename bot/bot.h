#ifndef _BOT_H
#define _BOT_H

#include "includes.h"



class basicBot {
	protected:
		SOCKET sock;
		bool errorOccurred;  //this holds a bool telling if an error has occurred, and it will stop if it's true

		DWORD serverToken;
		DWORD clientToken;

		void* nls;

		void error(string message);

		bool initializeSocket();
		void loadConfig();

		static HINSTANCE bncsutilHinst;
		static HINSTANCE checkrevisionHinst;

		bool checkRevisionEx(string gameExe, string gameDll, string bnetDll, string valueString, DWORD* exeVersion_out, DWORD* checksum_out, string* exeInfo_out, string versionDll, string videoBin);
		bool kdQuick(DWORD* publicValue_out, DWORD* productValue_out, char* hashBuf_out);
		void doubleHashPassword(char* hashBuf_out);
		void hashPassword(string password, char* hashBuf_out);
		void nlsInit();
		void nlsGetA(char* buf);
		void nlsGetM1(char* buf, char* serverKey, char* salt);
		bool nlsAccountCreate(char* buf, DWORD bufLen);
		void nlsFree();

		void sendProtocol();
		void parsePacket(char* data, int len);
		void send0x50();
		void parse0x50(packetDebuffer* pdebuf);
		void send0x51(string dllFile, string valueString);
		void parse0x51(packetDebuffer* pdebuf);
		void send0x3A();
		void parse0x3A(packetDebuffer* pdebuf);
		void send0x3D();
		void parse0x3D(packetDebuffer* pdebuf);
		void send0xABC();
		void parse0x0A(packetDebuffer* pdebuf);
		void parse0x0F(packetDebuffer* pdebuf);
		void send0x59();
		void send0x53();
		void parse0x53(packetDebuffer* pdebuf);
		void send0x54(char* salt, char* serverKey);
		void parse0x54(packetDebuffer* pdebuf);
		void send0x52();
		void parse0x52(packetDebuffer* pdebuf);


	public:
		basicBot(string profile);
		~basicBot();

		string profileName;

		string username;
		string password;
		string cdKey;
		string product;

		string uniqueUsername;
		string statString;
		

		static bool initializeWinsock();
		static void cleanupWinsock();
		static bool loadLibraries();

		string getConfigString(string key, string def = "");
		int getConfigInt(string key, int def = 0);

		bool connect(string server, unsigned short port = 6112);
		void disconnect();
		void botMain();


		void eventChatShowUser(string username, string statstring, DWORD ping, DWORD flags);
		void eventChatUserJoin(string username, string statstring, DWORD ping, DWORD flags);
		void eventChatUserLeave(string username, string statstring, DWORD ping, DWORD flags);
		void eventChatWhisper(string username, string text, DWORD ping, DWORD flags);
		void eventChatTalk(string username, string text, DWORD ping, DWORD flags);
		void eventChatBroadcast(string username, string text, DWORD ping, DWORD flags);
		void eventChatChannel(string username, string text, DWORD ping, DWORD flags);
		void eventChatUserFlags(string username, string text, DWORD ping, DWORD flags);
		void eventChatWhisperSent(string username, string text, DWORD ping, DWORD flags);
		void eventChatChannelFull(string username, string text, DWORD ping, DWORD flags);
		void eventChatChannelDoesNotExist(string username, string text, DWORD ping, DWORD flags);
		void eventChatChannelRestricted(string username, string text, DWORD ping, DWORD flags);
		void eventChatInfo(string username, string text, DWORD ping, DWORD flags);
		void eventChatError(string username, string text, DWORD ping, DWORD flags);
		void eventChatEmote(string username, string text, DWORD ping, DWORD flags);

};

#endif