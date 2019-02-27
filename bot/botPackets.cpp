#include "bot.h"

void basicBot::parsePacket(char* data, int len) {

	packetDebuffer* pdebuf = new packetDebuffer(data, len);

	pdebuf->debuffByte();
	DWORD id = pdebuf->debuffByte();
	pdebuf->debuffWord();


	switch (id) {

		case 0x0:
			pdebuf->sendRaw(sock);
			break;

		case 0x25:
			pdebuf->sendRaw(sock);
			break;

		case 0x50:
			parse0x50(pdebuf);
			break;

		case 0x51:
			parse0x51(pdebuf);
			break;

		case 0x3A:
			parse0x3A(pdebuf);
			break;

		case 0x3D:
			parse0x3D(pdebuf);
			break;

		case 0x0A:
			parse0x0A(pdebuf);
			break;

		case 0x0F:
			parse0x0F(pdebuf);
			break;

		case 0x59:
			send0x59();
			break;

		case 0x53:
			parse0x53(pdebuf);
			break;

		case 0x54:
			parse0x54(pdebuf);
			break;

		case 0x52:
			parse0x52(pdebuf);
			break;

		//the ones we ignore
		case 0x0B:
		case 0x75:
			break;

		default:
			cout << "<-> Received 0x" << hex << id << endl;
			cout << len;
			//pdebuf->display();
			break;

	}

	delete pdebuf;

}

void basicBot::sendProtocol() {

	packetBuffer pbuf;

	pbuf.insertByte(0x1);
	pbuf.sendRaw(sock);

}

void basicBot::send0x50() {

	/*
	(DWORD) Protocol ID (0)
	(DWORD) Platform ID
	(DWORD) Product ID
	(DWORD) Version Byte
	(DWORD) Product language
	(DWORD) Local IP for NAT compatibility*
	(DWORD) Time zone bias*
	(DWORD) Locale ID*
	(DWORD) Language ID*
	(STRING) Country abreviation
	(STRING) Country
	*/

	packetBuffer pbuf;

	pbuf.insertDword(0);
	pbuf.insertDword(stringToDword("IX86"));
	pbuf.insertDword(stringToDword(product.c_str()));
	pbuf.insertDword(strtol(getConfigString("verByte").c_str(), 0, 16));
	pbuf.insertDword(0);
	pbuf.insertDword(0);
	pbuf.insertDword(0);
	pbuf.insertDword(0);
	pbuf.insertDword(0);
	pbuf.insertString("USA");
	pbuf.insertString("United States");

	pbuf.send(0x50, sock);

}

void basicBot::parse0x50(packetDebuffer* pdebuf) {

	/*
	(DWORD) Logon Type
	(DWORD) Server Token
	(DWORD) UDPValue *
	(FILETIME) MPQ filetime
	(STRING) IX86ver filename
	(STRING) ValueString
	*/

	//DWORD serverToken; //we need this later, so it is a protected variable in the bot class
	string dllFile; //the lockdown or version file
	string valueString; //the string containing the formula for hashing


	pdebuf->removeDword();
	serverToken = pdebuf->debuffDword();
	pdebuf->removeDword();
	pdebuf->removeDword();
	pdebuf->removeDword();
	dllFile = pdebuf->debuffString();
	valueString = pdebuf->debuffString();

	if (dllFile.compare(0, 8, "lockdown") == 0)
		dllFile = currentDir() + "\\hashes\\lockdown\\" + dllFile.substr(0, 16) + ".dll";

	send0x51(dllFile, valueString);

}

void basicBot::send0x51(string dllFile, string valueString) {

	/*
	(DWORD) Client Token
	(DWORD) EXE Version
	(DWORD) EXE Hash
	(DWORD) Number of CD-keys in this packet
	(BOOLEAN) Spawn CD-key

	For Each Key:

		(DWORD) Key Length
		(DWORD) CD-key's product value
		(DWORD) CD-key's public value
		(DWORD) Unknown (0)
		(DWORD) [5] Hashed Key Data


	(STRING) Exe Information
	(STRING) CD-Key owner name
	*/


	//variables we'll be calculating and sending for the hash
	string exeInfo; //the string with the file time and what not
	DWORD exeVersion; //the version of the game executable
	DWORD checksum; //the hash

	bool ret;  //just holds what the functions return


	string file1, file2, file3, videoBin;

	file1 = file2 = file3 = currentDir() + "\\hashes\\" + product + "\\";
	videoBin = currentDir() + "\\hashes\\lockdown\\" + product + ".bin";

	if (product.compare("D2DV") == 0 || product.compare("D2XP") == 0) {
		file1 += "Game.exe";
		file2 += "Bnclient.dll";
		file3 += "D2Client.dll";
	} else if (product.compare("STAR") == 0 || product.compare("SEXP") == 0) {
		file1 += "Starcraft.exe";
		file2 += "Storm.dll";
		file3 += "Battle.snp";
	} else if (product.compare("W2BN") == 0) {
		file1 += "Warcraft II BNE.exe";
		file2 += "Storm.dll";
		file3 += "Battle.snp";
	} else if (product.compare("WAR3") == 0 || product.compare("W3XP") == 0) {
		file1 += "War3.exe";
		file2 += "Storm.dll";
		file3 += "Game.dll";
	}



	ret = checkRevisionEx(file1, file2, file3, valueString, &exeVersion, &checksum, &exeInfo, dllFile, videoBin);

	if (! ret) {
		error("<!> The game files could not be hashed.");
		return;
	}
	




	//variables we need for hashing the keys
	DWORD publicValue;
	DWORD productValue;
	char hashBuf[20];
	clientToken = GetTickCount();  //since we need this later, it's a protected variable



	ret = kdQuick(&publicValue, &productValue, hashBuf);

	if (! ret) {
		error("<!> The CD key could not be decoded.");
		return;
	}




	//we're finally done, time to send the packet
	packetBuffer pbuf;

	pbuf.insertDword(clientToken);
	pbuf.insertDword(exeVersion);
	pbuf.insertDword(checksum);
	pbuf.insertDword(1);
	pbuf.insertDword(0);
	pbuf.insertDword(cdKey.length());
	pbuf.insertDword(productValue);
	pbuf.insertDword(publicValue);
	pbuf.insertDword(0);
	pbuf.insertRaw(hashBuf, 20);
	pbuf.insertString(exeInfo);
	pbuf.insertString("-= Useless =-");
	pbuf.send(0x51, sock);

}

void basicBot::parse0x51(packetDebuffer* pdebuf) {

	/*
	(DWORD)		 Result
	(STRING) 	 Additional Information
	*/

	DWORD result = pdebuf->debuffDword();


	switch (result) {

		case 0:
			if (product.compare("WAR3") == 0 || product.compare("W3XP") == 0)
				send0x53();
			else
				send0x3A();
			break;

		case 0x100:
			error("<!> Game version must be upgraded with the MPQ specified in " + pdebuf->debuffString() + ".");
			break;

		case 0x101:
			error("<!> Invalid game version.");
			break;

		case 0x102:
			error("<!> Game version must be downgraded with the MPQ specified in " + pdebuf->debuffString() + ".");
			break;

		case 0x200:
			error("<!> Invalid CD key.");
			break;

		case 0x201:
			error("<!> CD key in use by " + pdebuf->debuffString() + ".");
			break;

		case 0x202:
			error("<!> CD key is banned.");
			break;

		case 0x203:
			error("<!> Your CD key is for the wrong product.");
			break;

		default:
			error("<!> Battle.net returned an unknown result in 0x51.");
			cout << "The result it returned is 0x" << hex << result << endl;
			cout << "Additional info: " << pdebuf->debuffString() << endl;
			break;

	}

}

void basicBot::send0x3A() {

	/*
	(DWORD)		 Client Token
	(DWORD)		 Server Token
	(DWORD[5])	 Password Hash
	(STRING) 	 Username
	*/


	char hashBuf[20];
	doubleHashPassword(hashBuf);


	packetBuffer pbuf;

	pbuf.insertDword(clientToken);
	pbuf.insertDword(serverToken);
	pbuf.insertRaw(hashBuf, sizeof(hashBuf));
	pbuf.insertString(username);
	pbuf.send(0x3A, sock);

}

void basicBot::parse0x3A(packetDebuffer* pdebuf) {

	/*
	(DWORD)		 Result
	(STRING) 	 Reason
	*/


	DWORD result = pdebuf->debuffDword();


	switch (result) {

		case 0:
			{
				packetBuffer pbuf;

				pbuf.insertDword(stringToDword("bnet"));
				pbuf.send(0x14, sock);
			}

			send0xABC();
			break;

		case 0x1:
			cout << "<!> Account does not exist!" << endl;
			send0x3D();
			break;

		case 0x2:
			error("<!> The password was incorrect.");
			break;

		default:
			error("<!> 0x3A return an unknown result!");
			break;

	}

}

void basicBot::send0x3D() {

	/*
	(DWORD[5])	 Password Hash
	(STRING) 	 Username
	*/


	cout << "Creating account..." << endl;


	char hashBuf[20];
	hashPassword(password, hashBuf);


	packetBuffer pbuf;

	pbuf.insertRaw(hashBuf, sizeof(hashBuf));
	pbuf.insertString(username);
	pbuf.send(0x3D, sock);

}

void basicBot::parse0x3D(packetDebuffer* pdebuf) {

	/*
	(DWORD)		 Status
	(STRING) 	 Account name suggestion
	*/


	DWORD status = pdebuf->debuffDword();


	switch (status) {

		case 0:
			cout << "<-> Account successfully created!" << endl;
			send0x3A();
			break;

		case 2:
			error("<!> Username contained illegal characters!");
			break;

		case 3:
			error("<!> Username contained an illegal word!");
			break;

		case 4:
			error("<!> Username already exists!");
			break;

		case 5:
			error("<!> Username did not contain enough alphanumeric characters!");
			break;

		default:
			error("<!> 0x3D returned an unknown result!");
			break;

	}

}

void basicBot::send0xABC() {

	/*
	0x0A
	(STRING) 	 *Username
	(STRING) 	 **Statstring

	0x0B
	(DWORD)		 Product ID

	0x0C
	(DWORD)		 Flags
	(STRING) 	 Channel
	*/

	
	packetBuffer pbuf;


	pbuf.insertString(username);
	pbuf.insertByte(0);
	pbuf.send(0x0A, sock);

	pbuf.insertDword(stringToDword(product.c_str()));
	pbuf.send(0x0B, sock);

	pbuf.insertDword(0x2);
	pbuf.insertString(getConfigString("homeChannel"));
	pbuf.send(0x0C, sock);

}

void basicBot::parse0x0A(packetDebuffer* pdebuf) {

	/*
	(STRING) 	 Unique Username
	(STRING) 	 Statstring
	(STRING) 	 Account name
	*/

	uniqueUsername = pdebuf->debuffString();
	statString = pdebuf->debuffString();

	uniqueUsername = uniqueUsername.substr(0, 1).compare("*") == 0 ? uniqueUsername.substr(1) : uniqueUsername;

	cout << profileName << ": Logged in as " << uniqueUsername << "!" << endl;

}

void basicBot::parse0x0F(packetDebuffer* pdebuf) {

	/*
	(DWORD)		 Event ID
	(DWORD)		 User's Flags
	(DWORD)		 Ping
	(DWORD)		 IP Address (Defunct)
	(DWORD)		 Account number (Defunct)
	(DWORD)		 Registration Authority (Defunct)
	(STRING) 	 Username
	(STRING) 	 Text

	[0x01] 	EID_SHOWUSER: 	User in channel
	[0x02] 	EID_JOIN: 	User joined channel
	[0x03] 	EID_LEAVE: 	User left channel
	[0x04] 	EID_WHISPER: 	Recieved whisper
	[0x05] 	EID_TALK: 	Chat text
	[0x06] 	EID_BROADCAST: 	Server broadcast
	[0x07] 	EID_CHANNEL: 	Channel information
	[0x09] 	EID_USERFLAGS: 	Flags update
	[0x0A] 	EID_WHISPERSENT: 	Sent whisper
	[0x0D] 	EID_CHANNELFULL: 	Channel full
	[0x0E] 	EID_CHANNELDOESNOTEXIST: 	Channel doesn't exist
	[0x0F] 	EID_CHANNELRESTRICTED: 	Channel is restricted
	[0x12] 	EID_INFO: 	Information
	[0x13] 	EID_ERROR: 	Error message
	[0x17] 	EID_EMOTE: 	Emote
	*/

	DWORD eventId = pdebuf->debuffDword();
	DWORD flags = pdebuf->debuffDword();
	DWORD ping = pdebuf->debuffDword();
	pdebuf->removeDword();
	pdebuf->removeDword();
	pdebuf->removeDword();
	string user = pdebuf->debuffString();
	string text = pdebuf->debuffString();

	user = user.substr(0, 1).compare("*") == 0 ? user.substr(1) : user;

	switch (eventId) {

		case 0x01:
			eventChatShowUser(user, text, ping, flags);
			break;

		case 0x02:
			eventChatUserJoin(user, text, ping, flags);
			break;

		case 0x03:
			eventChatUserLeave(user, text, ping, flags);
			break;

		case 0x04:
			eventChatWhisper(user, text, ping, flags);
			break;

		case 0x05:
			eventChatTalk(user, text, ping, flags);
			break;

		case 0x06:
			eventChatBroadcast(user, text, ping, flags);
			break;

		case 0x07:
			eventChatChannel(user, text, ping, flags);
			break;

		case 0x09:
			eventChatUserFlags(user, text, ping, flags);
			break;

		case 0x0A:
			eventChatWhisperSent(user, text, ping, flags);
			break;

		case 0x0D:
			eventChatChannelFull(user, text, ping, flags);
			break;

		case 0x0E:
			eventChatChannelDoesNotExist(user, text, ping, flags);
			break;

		case 0x0F:
			eventChatChannelRestricted(user, text, ping, flags);
			break;

		case 0x12:
			eventChatInfo(user, text, ping, flags);
			break;

		case 0x13:
			eventChatError(user, text, ping, flags);
			break;

		case 0x17:
			eventChatEmote(user, text, ping, flags);
			break;

		default:
			cout << "Chat event: 0x" << hex << eventId << endl;
			break;

	}

}

void basicBot::send0x59() {

	cout << "Sending email registration..." << endl;

	string email = getConfigString("email");


	packetBuffer pbuf;

	pbuf.insertString(email);
	pbuf.send(0x59, sock);

	cout << "<-> Account registered to " << email << "!" << endl;

}

void basicBot::send0x53() {

	nlsInit();

	if ((int)nls == 0)
		return;


	char buf[32];

	nlsGetA(buf);


	packetBuffer pbuf;

	pbuf.insertRaw(buf, 32);
	pbuf.insertString(username);
	pbuf.send(0x53, sock);

}

void basicBot::parse0x53(packetDebuffer* pdebuf) {

	DWORD status = pdebuf->debuffDword();
	char salt[32];
	pdebuf->debuffRaw(salt, 32);
	char serverKey[32];
	pdebuf->debuffRaw(serverKey, 32);


	switch (status) {

		case 0:
			send0x54(salt, serverKey);
			break;

		case 0x01:
			cout << "<!> Account does not exist!" << endl;
			send0x52();
			return;

		case 0x05:
			error("<!> Account needs to be upgraded to the new login system!");
			nlsFree();
			return;

		default:
			error("<!> 0x53 returned an unexpected result!");
			nlsFree();
			return;

	}

}

void basicBot::send0x54(char* salt, char* serverKey) {

	char buf[20];
	nlsGetM1(buf, serverKey, salt);


	packetBuffer pbuf;

	pbuf.insertRaw(buf, 20);
	pbuf.send(0x54, sock);

	nlsFree();

}

void basicBot::parse0x54(packetDebuffer* pdebuf) {

	DWORD status = pdebuf->debuffDword();
	char passProof[20];
	pdebuf->debuffRaw(passProof, 20);

	switch (status) {

		case 0x0E:
			send0x59();
		case 0:
			send0xABC();
			break;

		case 0x02:
			error("<!> The password was incorrect.");
			break;

		case 0x0F:
		default:
			error("<!> 0x54 returned an unknown result!");
			cout << pdebuf->debuffString() << endl;
			break;

	}

}

void basicBot::send0x52() {

	cout << "Creating account..." << endl;

	int bufLen = username.length() + 65;
	char* buf = new char[bufLen];


	if (! nlsAccountCreate(buf, bufLen))
		goto freeMemory;

	{
	packetBuffer pbuf;

	pbuf.insertRaw(buf, bufLen);
	pbuf.send(0x52, sock);
	}


freeMemory:
	delete[] buf;

}

void basicBot::parse0x52(packetDebuffer* pdebuf) {

	DWORD result = pdebuf->debuffDword();

	if (result == 0) {
		cout << "<-> Account successfully created!" << endl;
		send0x53();
	} else {
		error("<!> Account creation failed!");
		nlsFree();
	}

}