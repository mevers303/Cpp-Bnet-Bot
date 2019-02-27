#include "bot.h"
#include <vector>

bool readyForNext;
HANDLE* threads;
int threadCount;

DWORD WINAPI threadProc(void* param) {

	string profileName = *reinterpret_cast<string*>(param);

	basicBot bot(profileName);

	readyForNext = true;

	bot.botMain();

	return 0;

}

void loadProfiles() {

	string filename = currentDir() + "\\profiles.ini";
	vector<string> profiles;

	char buf[10240];
	GetPrivateProfileSectionNames(buf, sizeof(buf), filename.c_str());


	int currentPos = 0;

	while (buf[currentPos] != 0) {

		string profileName = buf + currentPos;
		profiles.push_back(profileName);

		currentPos += profileName.length() + 1;

	}

	threadCount = profiles.size();
	threads = new HANDLE[threadCount];

	for (int i = 0; i < threadCount; i++) {

		readyForNext = false;

		string profileName = profiles[i];
		threads[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, &profileName, 0, 0);

		while (! readyForNext) {
			Sleep(1);
		}

	}

	profiles.clear();

}

int main() {

	if (! basicBot::initializeWinsock())
		goto programEnd;
	if (! basicBot::loadLibraries())
		goto programEnd;

	cout << endl;

	loadProfiles();

	
	WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);

programEnd:
	cout << endl << "Press ENTER to continue..." << endl;
	cin.get();
	cout << endl;

}