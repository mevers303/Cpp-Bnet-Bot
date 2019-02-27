#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include <string.h>
#include <windows.h>
#include "buffer.h"

#define GetBYTE(Pointer) (*(BYTE*)(Pointer))
#define GetWORD(Pointer) (*(WORD*)(Pointer))
#define GetDWORD(Pointer) (*(DWORD*)(Pointer)) 



#define BUFFSIZE 1024


class packetBuffer {
	protected:
		BYTE buf[BUFFSIZE];
		char* currentPos;

	public:
		packetBuffer() {
			currentPos = buf + 4;
			zeroMemory(buf, BUFFSIZE);
		}

		void insert(void* data, int len) {
			copyMemory(currentPos, data, len);
			currentPos += len;
		}

		void insert(char* data) {
			insert(data, strlen(data) + 1);
		}

		void insert(BYTE data) {
			insert(&data, sizeof(data));
		}

		void insert(WORD data) {
			insert(&data, sizeof(data));
		}

		void insert(DWORD data) {
			insert(&data, sizeof(data));
		}

		bool send(BYTE id, SOCKET s, int flags = 0) {

			int len = currentPos - buf;

			buf[0] = 0xFF;
			buf[1] = id;

			
			copyMemory(buf + 2, (WORD)len, sizeof(WORD));

			::send(s, buf, len, flags);

		}

};

#endif