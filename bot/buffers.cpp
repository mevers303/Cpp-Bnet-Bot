#include "buffers.h"

packetBuffer::packetBuffer() {

	buf = new char[0];

	len = currentPos = 0;

}

packetBuffer::~packetBuffer() {

	delete[] buf;

}

void packetBuffer::clear() {

	delete[] buf;
	buf = new char[0];

	len = currentPos = 0;

}

void packetBuffer::setCurrentPos(int pos) {

	if (pos < 0)
		pos = 0;

	if (pos > len)
		pos = len;


	currentPos = pos;

}

char* packetBuffer::getBuf() {

	return buf;

}

int packetBuffer::getLen() {

	return len;

}

int packetBuffer::getCurrentPos() {

	return currentPos;

}

void packetBuffer::insertRaw(const void* data, int size) {	

	if (currentPos + size > len) {

		int newLen = len + size;
		char* newBuf = new char[newLen];

		memcpy(newBuf, buf, len);
		delete[] buf;

		len = newLen;
		buf = newBuf;

	}


	memcpy(buf + currentPos, data, size);

	currentPos += size;

}

void packetBuffer::insertString(string data) {

	insertRaw(data.c_str(), data.length() + 1);

}

void packetBuffer::insertByte(BYTE data) {

	insertRaw(&data, 1);

}

void packetBuffer::insertWord(WORD data) {

	insertRaw(&data, 2);

}

void packetBuffer::insertDword(DWORD data) {

	insertRaw(&data, 4);

}

bool packetBuffer::send(BYTE id, SOCKET s) {
	
	int newLen = len + 4;
	char* newBuf = new char[newLen];

	memcpy(newBuf + 4, buf, len);
	delete[] buf;
	
	buf = newBuf;
	len = newLen;


	currentPos = 0;

	insertByte(0xFF);
	insertByte(id);
	insertWord(len);

	return sendRaw(s);

}

bool packetBuffer::sendRaw(SOCKET s) {

	int result = 0;

	while (result < len) {
				
		int tempResult = ::send(s, buf + result, len - result, 0);

		if (tempResult == SOCKET_ERROR)
			return false;

		result += tempResult;

	}


	clear();

	return true;

}

void packetBuffer::display() {

	for (int i = 0; i < len; i++) {

		if (i % 15 == 0)
			cout << endl;

		unsigned int temp = 0;
		memcpy(&temp, buf + i, 1);

		cout << (temp > 0xF ? "" : "0") << hex << temp << " ";

	}


	cout << endl;

}

packetDebuffer::packetDebuffer(const char* data, int size) {

	insertRaw(data, size);
	setCurrentPos(0);

}

void packetBuffer::removeRaw(int size) {
	currentPos += size;
}

void packetBuffer::removeString() {

	while (buf[currentPos++] != 0) {
		//do nothing
	}

}

void packetBuffer::removeByte() {
	currentPos += 1;
}

void packetBuffer::removeWord() {
	currentPos += 2;
}

void packetBuffer::removeDword() {
	currentPos += 4;
}

void packetBuffer::debuffRaw(void* destination, int size) {

	if (currentPos + size > len) {
		memset(destination, 0, size);
		return;
	}

	memcpy(destination, buf + currentPos, size);
	currentPos += size;

}

string packetBuffer::debuffString() {

	char temp[1024];
	int i;

	for (i = 0; (buf[currentPos] != 0) && (currentPos <= len); i++)
		temp[i] = buf[currentPos++];

	temp[i] = 0;
	currentPos++;

	string val = temp;
	return val;

}

BYTE packetBuffer::debuffByte() {
	BYTE val;
	debuffRaw(&val, 1);
	return val;
}

WORD packetBuffer::debuffWord() {
	WORD val;
	debuffRaw(&val, 2);
	return val;
}

DWORD packetBuffer::debuffDword() {
	DWORD val;
	debuffRaw(&val, 4);
	return val;
}
