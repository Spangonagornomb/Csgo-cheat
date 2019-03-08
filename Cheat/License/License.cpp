#include "License.h"

CLicense::DllArgs* CLicense::pDllArgs = nullptr;



bool CLicense::Check()
{
	VMP_ULTRA("LicenseCheck");

	auto LicCh = [&]()
	{
		try
		{
			CInternet Inet(("dceowysgpv.info"));

			CCrypt cr;
			ifstream file;

			string fStr = MainSettings().key;

			string ServerBuffer = "";
			string zUrl = ("/XONE/Chel.php");
			zUrl += ("?K=") + OCrypt(fStr) + ("&CTP=") + OCrypt(CHEAT_TYPE);
			ServerBuffer = Inet.GetData(zUrl);
			FixStr(ServerBuffer);

			if (ServerBuffer.length() <= 10) { return false; }

			ServerBuffer = base64_decode(cr.I(base64_decode(cr.I(base64_decode(ServerBuffer)))));

			if (ServerBuffer.find(("MSQL:")) != -1)
			{
				ADD_LOG("License: MSQL Error\n");
				return false;
			}

			string ErrorStr = "";
			bool ReadError = false;
			for (int i(0); i < (int)ServerBuffer.length(); i++)
			{
				if (ServerBuffer[i] == '[') { ReadError = true; continue; }
				if (ServerBuffer[i] == ']') { break; }
				if (ReadError) { ErrorStr += ServerBuffer[i]; }
			}

			if (ErrorStr == ("NULL")) { ADD_LOG("License: NULL\n"); return false; }
			if (ErrorStr == ("EMPT")) { ADD_LOG("License: EMPT\n"); return false; }

			string ServerMD5 = HighlightText(("_MD5"), ServerBuffer);
			string ServerEndDate = HighlightText(("_END_D"), ServerBuffer);
			string ServerCheatType = HighlightText(("_CHET"), ServerBuffer);
			string ServerLogin = HighlightText(("_LOG"), ServerBuffer);
			string ServerBan = HighlightText(("_BAN"), ServerBuffer);
			string ServerLicFreez = HighlightText(("_FREZ"), ServerBuffer);

			if (ServerLogin != MainSettings().UserName) { return false; }
			//if (ServerCheatType != CHEAT_TYPE) { ADD_LOG("License: ErrType\n"); return false; }
			if (ServerBan == ("1")) { ADD_LOG("License: Ban\n"); return false; }

			int fI = ServerBuffer.find(fStr);

			if (fI != -1)
			{
				ADD_LOG("License: Sucessful\n");
				return true;
			}

			ADD_LOG("License: Error\n");
			return false;
			VMP_END
		}
		catch (const exception& e)
		{
		}
	};

	return LicCh();
}


bool CLicense::CheckMemes()
{
	if (MainSettings().UserName != (string)pDllArgs->Name)
	{
		ADD_LOG("License: Error\n");
		return false;
	}

	ADD_LOG("License: CheckArgs sucessful\n");
	return true;
}

void CLicense::GetCheatUser()
{
	VMP_ULTRA("GetCheatUser")
	string path;
	string out;
	string MainBuffer;
	string ke = "";

	int lastpos = 0;

	std::ifstream file;

	path += GetSysDrv();
	path += PATH_TO_CHEAT_SETTINGS;
	path += "User\\";
	path += "UserName";

	file.open(path);

	CCrypt cr;

	if (file)
	{
		file >> out;
		file.close();

		MainBuffer = cr.I(cr.I(out));

		MainSettings().key = HighlightText(("_MD5_"), MainBuffer);
		MainSettings().UserName = HighlightText(("_USER_NAME_"), MainBuffer);
		MainSettings().EndDate = HighlightText(("_DATA_USER_"), MainBuffer);
		MainSettings().Days = HighlightText(("_END_USER_"), MainBuffer);
		MainSettings().id = HighlightText(("_ID_USER_"), MainBuffer);

	}
	VMP_END
}

string CLicense::GetSysDrv()
{
	return getenv("SystemDrive");
}

string  CLicense::OCrypt(string In)
{
	CCrypt cr;
	string bKF = base64_encode((const unsigned char *)cr.O(In).c_str(), In.length());
	string bbKF = base64_encode((const unsigned char *)bKF.c_str(), bKF.length());
	return bbKF;
}
void  CLicense::FixStr(std::string &str)
{
	std::string _FixStr = "";
	static bool ReadFix = false;

	for (int i(0); i < (int)str.length(); i++)
	{
		if (str[i] == '[') { ReadFix = true; continue; }
		if (str[i] == ']') { break; }

		if (ReadFix)
			_FixStr += str[i];
	}

	str = _FixStr;
}

std::string CLicense::HighlightText(std::string finde, std::string buffe)
{
	std::string ret = "";

	bool ToRead = false;

	int findHighlight = buffe.find(finde);
	if (findHighlight != -1)
	{

		for (int i(findHighlight); i < (int)buffe.length(); i++)
		{
			if (buffe[i] == ')')
			{
				return ret;
				break;
			}
			if (buffe[i - 1] == '(')
			{
				ToRead = true;
			}
			if (ToRead)
			{
				ret += buffe[i];
			}
		}
	}
	else
	{
		ret = "";
	}

	buffe.clear();
	return ret;
}

std::string CCrypt::I(std::string str) {
	std::string ret;

	for (int i(0); i < (int)str.length(); i++) {
		for (int k(0); k < (int)alphabet.length(); k++) {
			if (str[i] == crypt[k]) {
				ret += alphabet[k];
			}
		}
	}
	return ret;
}

std::string CCrypt::O(std::string str) {
	std::string ret;

	for (int i(0); i < (int)str.length(); i++) {
		for (int k(0); k < (int)alphabet.length(); k++) {
			if (str[i] == alphabet[k]) {
				ret += crypt[k];
			}
		}
	}
	return ret;
}

std::string CCrypt::IEx(std::string &str) {

	for (size_t i(0); i < str.length(); i++) {
		for (size_t k(0); k < alphabet.length(); k++) {
			if (str[i] == crypt[k]) {
				str[i] = alphabet[k];
			}
		}
	}
	return str;
}

std::string CCrypt::OEx(std::string &str) {

	for (size_t i(0); i < str.length(); i++) {
		for (size_t k(0); k < alphabet.length(); k++) {
			if (str[i] == alphabet[k]) {
				str[i] = crypt[k];
			}
		}
	}
	return str;
}

std::vector<int> CCrypt::VO(std::string str, int ob)
{
	std::vector<int> out;
	for (int i(0); i < ob; i++)
		str = O(str);

	for (int i(0); i < (int)str.length(); i++)
		for (int k(0); k < (int)crypt.length(); k++)
			if (str[i] == crypt[k])
				out.insert(out.end(), k);

	return out;
}

std::string CCrypt::VI(std::vector<int> vec, int ob) {

	std::string ret;

	for (int i(0); i < (int)vec.size(); i++)
		for (int k(0); k < (int)crypt.length(); k++)
			if (vec[i] == k)
				ret += crypt[k];

	for (int i(0); i < ob; i++)
		ret = I(ret);

	return ret;
}

string CInternet::CreateError(string ret)
{
	int WinLastError = FastCall::G().t_GetLastError();
	return ret;
}

string CInternet::GetData(string URL)
{
	VMP_ULTRA("GetData");
	string OutData = "";

	CHAR szBuffer[1024] = { 0 };
	DWORD dwRead = 0;

	char* szHeaders = "Content-Type: text/html\r\nUser-Agent: License";
	char szRequest[1024] = { 0 };

	HINTERNET hIntSession = InternetOpenA("", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hIntSession)
		return CreateError(OutData);

	HINTERNET hHttpSession = InternetConnectA(hIntSession, HostUrl.c_str(), 80, 0, 0, INTERNET_SERVICE_HTTP, 0, NULL);
	if (!hHttpSession)
		return CreateError(OutData);

	HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", URL.c_str(), 0, 0, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (!hHttpRequest)
		return CreateError(OutData);

	if (!HttpSendRequestA(hHttpRequest, szHeaders, strlen(szHeaders), szRequest, strlen(szRequest)))
		return CreateError(OutData);

	while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
		OutData.append(szBuffer, dwRead);

	if (hHttpRequest)InternetCloseHandle(hHttpRequest);
	if (hHttpSession)InternetCloseHandle(hHttpSession);
	if (hIntSession)InternetCloseHandle(hIntSession);

	return OutData;
	VMP_END
}