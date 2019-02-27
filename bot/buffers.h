#ifndef _BUFFERS_H_
#define _BUFFERS_H_


/*#include <iostream>
#include <string>
#include <memory.h>
#include "winsock2.h"*/
#include "includes.h"
using namespace std;


class packetBuffer {
	protected:
		char* buf;  //holds all the data
		int len;  //the length of the buffer
		int currentPos;  //the current position that will be written at

	public:
		packetBuffer();  //sets currentPos and len to 0, and gets a pointer to nothing
		~packetBuffer();  //deletes the buffer

		void clear();  //clears the buffer and resets currentPos and len
		void setCurrentPos(int pos);  //sets the current position to be written to
									  //will be set to 0 if pos < 0, and set to len if pos > len (the end of the buffer)

		char* getBuf();  //returns the pointer to the buffer
		int getLen();  //returns the length of the buffer (without headers)
		int getCurrentPos();  //returns the current position in the buffer

		void insertRaw(const void* data, int size);  //inserts raw bytes to the buffer
		void insertString(string data);  //inserts a null terminated string to the buffer
		void insertByte(BYTE data);  //inserts a byte into the buffer
		void insertWord(WORD data);  //inserts a word (two bytes)
		void insertDword(DWORD data);  //inserts a dword (four bytes)

		void removeRaw(int size);  //progresses the pointer size bytes
		void removeString();  //moves the pointer past the null terminator
		void removeByte();  //moves the pointer ahead one byte
		void removeWord();  //moves ahead two bytes
		void removeDword();  //moves ahead four

		void debuffRaw(void* destination, int size);  //copies size bytes to destination
		string debuffString();  //returns and removes a null terminated string
		BYTE debuffByte();  //returns a byte
		WORD debuffWord();  //returns a Word
		DWORD debuffDword();  //returns a dword

		bool sendRaw(SOCKET s);  //sends the data in buffer without adding a header first
		bool send(BYTE id, SOCKET s);  //adds the packet header to the beginning of the buffer, then sends it

		void display();  //dumps the buffer to cout

};



class packetDebuffer: public packetBuffer {
	public:
		packetDebuffer(const char* data, int size);
};

#endif  // _BUFFERS_H_
