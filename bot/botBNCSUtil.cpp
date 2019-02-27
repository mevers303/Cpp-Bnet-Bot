#include "bot.h"

HINSTANCE basicBot::bncsutilHinst;
HINSTANCE basicBot::checkrevisionHinst;

bool basicBot::checkRevisionEx(string gameExe, string gameDll, string bnetDll, string valueString, DWORD* exeVersion_out, DWORD* checksum_out, string* exeInfo_out, string versionDll, string videoBin) {

	bool ret = false;

	typedef BOOL (WINAPI *checkRevision_type)(LPCTSTR lpszFileName1, LPCTSTR lpszFileName2, LPCTSTR lpszFileName3, LPCTSTR lpszValueString, DWORD * lpdwVersion, DWORD * lpdwChecksum, LPSTR lpReturn, LPCTSTR lpszDLL, LPCTSTR lpszPathToLD01, LPCTSTR PathToVideoBin);
	checkRevision_type checkRevision;

	DWORD exeVersion = 0;
	DWORD checksum = 0;
	string exeInfo = "";



	checkRevision = (checkRevision_type)GetProcAddress(checkrevisionHinst, "CheckRevisionEx");

	if (checkRevision == NULL) {
		error("<!> Could not load the CheckRevisionEx function from CheckRevision.dll!");
		goto freeAndReturn;
	}



	char temp[256]; //for LPSTR lpReturn

	BOOL checkRevision_ret = checkRevision(gameExe.c_str(), gameDll.c_str(), bnetDll.c_str(),
							 valueString.c_str(), &exeVersion, &checksum,
							 temp, versionDll.c_str(), 0, videoBin.c_str());

	if (! checkRevision_ret) {
		error("<!> CheckRevisionEx failed!");
		goto freeAndReturn;
	}


	exeInfo = temp;
	ret = true;

	

freeAndReturn:

	*exeVersion_out = exeVersion;
	*checksum_out = checksum;
	*exeInfo_out = exeInfo;


	return ret;

}

bool basicBot::loadLibraries() {

	cout << "Loading libraries..." << endl;

	bncsutilHinst = LoadLibrary("BNCSUtil.dll");

	if (bncsutilHinst == NULL) {
		cout << "Could not load BNCSUtil.dll!" << endl;
		FreeLibrary(bncsutilHinst);
		return false;
	}

	checkrevisionHinst = LoadLibrary("Checkrevision.dll");

	if (checkrevisionHinst == NULL) {
		cout << "Could not load Checkrevision.dll!" << endl;
		FreeLibrary(bncsutilHinst);
		FreeLibrary(checkrevisionHinst);
		return false;
	}

	cout << "Connection libraries loaded!" << endl;
	return true;

}

bool basicBot::kdQuick(DWORD* publicValue_out, DWORD* productValue_out, char* hashBuf_out) {

	bool ret = false;

	typedef int (WINAPI *kd_quick_type)(const char* cd_key, DWORD client_token,
				   DWORD server_token, DWORD* public_value,
				   DWORD* product, char* hash_buffer, size_t buffer_len);
	kd_quick_type kd_quick;

	char hashBuf[20];
	memset(hashBuf, 0, sizeof(hashBuf));
	DWORD publicValue = 0;
	DWORD productValue = 0;


	kd_quick = (kd_quick_type)GetProcAddress(bncsutilHinst, "kd_quick");

	if (kd_quick == NULL) {
		error("<!> Could not load the kd_quick function from BNCSUtil.dll!");
		goto functionReturn;
	}


	
	int kdReturn = kd_quick(cdKey.c_str(), clientToken, serverToken, &publicValue, &productValue, hashBuf, sizeof(hashBuf));

	if (! kdReturn) {
		error("<!> kd_quick failed!");
		goto functionReturn;
	}



	ret = true;

	
functionReturn:

	*publicValue_out = publicValue;
	*productValue_out = productValue;
	memcpy(hashBuf_out, hashBuf, sizeof(hashBuf));

	return ret;

}

void basicBot::doubleHashPassword(char* hashBuf_out) {

	typedef int (WINAPI *doubleHash_type)(const char* password, DWORD clientToken, DWORD serverToken, char* outBuffer);
	doubleHash_type doubleHash;

	memset(hashBuf_out, 0, sizeof(hashBuf_out));



	doubleHash = (doubleHash_type)GetProcAddress(bncsutilHinst, "doubleHashPassword");

	if (doubleHash == NULL) {
		error("<!> Could not load the doubleHashPassword function from BNCSUtil.dll!");
		return;
	}


	
	doubleHash(password.c_str(), clientToken, serverToken, hashBuf_out);

}

void basicBot::hashPassword(string password, char* hashBuf_out) {

	typedef int (WINAPI *hash_type)(const char* password, char* outBuffer);
	hash_type hash;

	memset(hashBuf_out, 0, sizeof(hashBuf_out));


	hash = (hash_type)GetProcAddress(bncsutilHinst, "hashPassword");

	if (hash == NULL) {
		error("<!> Could not load the hashPassword function from BNCSUtil.dll!");
		return;
	}


	
	hash(password.c_str(), hashBuf_out);

}

void basicBot::nlsInit() {

	typedef void* (WINAPI *nls_init_type)(const char* username, const char* password);
	nls_init_type nls_init;



	nls_init = (nls_init_type)GetProcAddress(bncsutilHinst, "nls_init");

	if (nls_init == NULL) {
		error("<!> Could not load the nls_init function from BNCSUtil.dll!");
		return;
	}


	
	nls = nls_init(username.c_str(), password.c_str());

	if ((int)nls == 0)
		error("Failed to initialize NLS!");

}

void basicBot::nlsGetA(char* buf) {

	typedef void* (WINAPI *nls_get_A_type)(void* nls, char* out);
	nls_get_A_type nls_get_A;


	nls_get_A = (nls_get_A_type)GetProcAddress(bncsutilHinst, "nls_get_A");

	if (nls_get_A == NULL) {
		error("<!> Could not load the nls_get_A function from BNCSUtil.dll!");
		return;
	}


	nls_get_A(nls, buf);

}

void basicBot::nlsGetM1(char* buf, char* serverKey, char* salt) {

	typedef void* (WINAPI *nls_get_M1_type)(void* nls, char* out, const char* B, const char* salt);
	nls_get_M1_type nls_get_M1;


	nls_get_M1 = (nls_get_M1_type)GetProcAddress(bncsutilHinst, "nls_get_M1");

	if (nls_get_M1 == NULL) {
		error("<!> Could not load the nls_get_M1 function from BNCSUtil.dll!");
		return;
	}


	nls_get_M1(nls, buf, serverKey, salt);

}

bool basicBot::nlsAccountCreate(char* buf, DWORD bufLen) {

	typedef DWORD (WINAPI *nls_account_create_type)(void* nls, char* buf, unsigned long bufSize);
	nls_account_create_type nls_account_create;


	nls_account_create = (nls_account_create_type)GetProcAddress(bncsutilHinst, "nls_account_create");

	if (nls_account_create == NULL) {
		error("<!> Could not load the nls_account_create function from BNCSUtil.dll!");
		return false;
	}


	DWORD ret = nls_account_create(nls, buf, bufLen);

	if (! ret) {
		error("<!> nls_account_create failed!");
		return false;
	}

	return true;

}

void basicBot::nlsFree() {

	typedef void* (WINAPI *nls_free_type)(void* nls);
	nls_free_type nls_free;



	nls_free = (nls_free_type)GetProcAddress(bncsutilHinst, "nls_free");

	if (nls_free == NULL) {
		error("<!> Could not load the nls_free function from BNCSUtil.dll!");
		return;
	}


	
	nls = nls_free(nls);

	if ((int)nls == 0)
		error("Failed to free NLS!");

}