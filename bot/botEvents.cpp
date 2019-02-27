#include "bot.h"


void basicBot::eventChatShowUser(string username, string statstring, DWORD ping, DWORD flags) {
	//cout << "<-> " << username << " is present in the channel." << endl;
	//if (username.compare(uniqueUsername) == 0)
	//	cout << endl;
}

void basicBot::eventChatUserJoin(string username, string statstring, DWORD ping, DWORD flags) {
	//cout << "<-> " << username << " has joined the channel." << endl;
}

void basicBot::eventChatUserLeave(string username, string statstring, DWORD ping, DWORD flags) {
	//cout << "<-> " << username << " has left the channel." << endl;
}

void basicBot::eventChatWhisper(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<FROM " << username << "> " << text << endl;
}

void basicBot::eventChatTalk(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<" << username << "> " << text << endl;
}

void basicBot::eventChatBroadcast(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<!> SERVER MESSAGE\n\t" << text << endl;
}

void basicBot::eventChatChannel(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<-> Joined channel " << text << "!" << endl << endl;
}

void basicBot::eventChatUserFlags(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<-> Flags Update - " << username << ": 0x" << hex << flags << endl;
}

void basicBot::eventChatWhisperSent(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<TO " << username << "> " << text << endl;
}

void basicBot::eventChatChannelFull(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<!> Channel is full!" << endl;
}

void basicBot::eventChatChannelDoesNotExist(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<!> Channel does not exist!" << endl;
}

void basicBot::eventChatChannelRestricted(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<!> Channel is restricted!" << endl;
}

void basicBot::eventChatInfo(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<-> " << text << endl;
}

void basicBot::eventChatError(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<!> " << text << endl;
}

void basicBot::eventChatEmote(string username, string text, DWORD ping, DWORD flags) {
	//cout << "<" << username << " " << text << ">" << endl;
}