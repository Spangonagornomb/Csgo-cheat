#include "CloudSettings.h"

#include "../License/base64.h"
#include "../X1API/HTTP/CHTTPSession.h"

//#pragma comment(lib, "zlib.lib")
//#include <gzip/compress.hpp>

size_t GetControlSum(const std::string &str)
{
	size_t ret = 0;

	for (auto s : str)
		ret += (int)s;

	return ret;
}

std::string GetHashStr(const std::string &str)
{
	size_t CSum = GetControlSum(str);
	size_t len = str.length();
	size_t center = len / 2;
	size_t szHash = 1;

	for (size_t i(0); i < len; i++)
	{
		int pos1 = center - i;
		int pos3 = center + i;

		int ch1 = (int)str[pos1 < 0 ? 0 : pos1];
		int ch2 = (int)str[i];
		int ch3 = (int)str[pos3 >= len ? len - 1 : pos3];

		int fh = (int)str[0] + ch3 + CSum + ((ch1 + ch2 + ch3) / 2);

		szHash += (fh == 0 ? 1 : fh) * (ch3 * (ch1 + (i * ch2))) * (i * ch1) + i;
	}

	std::string ret = std::to_string(szHash);

	std::string Nums = "1234567890-";
	std::string Alphs = "1B3D5H7G90R";

	for (int i(0); i < ret.length(); i++)
		for (int j(0); j < Nums.length(); j++)
			if (Nums[j] == ret[i])
				ret[i] = Alphs[j];

	return ret;
}


std::string base64_encode__(const std::string &in) {

	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}
void CCloudSets::Save(std::string _userdata, std::string _filename, std::string _settings)
{
	CHTTPSession Session;

	std::string userdata = _userdata;
	std::string filename = _filename;
	std::string pathhash = GetHashStr(_userdata + "/" + _filename);

	const char * pointer_settings = _settings.data();
	std::size_t size_settings = _settings.size();

	std::string settings = _settings;

	printf("%i\n", (int)settings.size());
	std::string Answer = "";
	std::string POSTDATA = "";

	POSTDATA += "userdata=" + base64_encode((const unsigned char *)userdata.c_str(), userdata.length()) + "&";
	POSTDATA += "filename=" + base64_encode((const unsigned char *)filename.c_str(), filename.length()) + "&";
	POSTDATA += "pathhash=" + base64_encode((const unsigned char *)pathhash.c_str(), pathhash.length()) + "&";
	POSTDATA += "settings=" + settings;

	//Session.Transmit(HTTP_POST, HTTP_CLEAR, ("dceowysgpv.info/XONE/SaveCfg.php"), Answer, POSTDATA, 0);

	printf("%s\n", Answer.c_str());
}
std::string HighlightText(std::string finde, std::string buffe)
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
			if (buffe[(i == 0 ? 0 : i - 1)] == '(')
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

std::string CCloudSets::Load(std::string _hash, std::string &_name)
{
	CHTTPSession Session;

	std::string Answer = "";
	std::string POSTDATA = "";

	POSTDATA += ("pathhash=") + base64_encode((const unsigned char *)_hash.c_str(), _hash.length());
	//Session.Transmit(HTTP_POST, HTTP_CLEAR, ("dceowysgpv.info/XONE/LoadCfg.php"), Answer, POSTDATA, 0);

	_name = HighlightText(("_CFG_NAME_"), Answer);

	if (_name.empty())
		return "";

	std::string start_str = ("<<<<<<[");
	std::string end_str = (">>>>>>[");

	size_t start_pos = Answer.find(start_str);
	size_t end_pos = Answer.find(end_str);

	if (Answer.find(start_str) != std::string::npos && Answer.find(end_str) != std::string::npos)
	{
		size_t diff = end_pos - (start_pos + start_str.size());
		Answer = Answer.substr(start_pos + start_str.size(), diff);
	}
	else
		return "";

	//printf("%s\n", Answer.c_str());

	return Answer;
}



std::vector<CCloudSets::cfg_info_s> CCloudSets::UpdateList(std::string _user)
{
	std::vector<cfg_info_s> list_entry;

	CHTTPSession Session;

	std::string Answer = "";
	std::string POSTDATA = "";

	POSTDATA += ("user=") + base64_encode((const unsigned char *)_user.c_str(), _user.length());
	//Session.Transmit(HTTP_POST, HTTP_CLEAR, ("dceowysgpv.info/XONE/GetCfgList.php"), Answer, POSTDATA, 0);

	int i = 0;

	while (true)
	{
		CCloudSets::cfg_info_s entry;

		entry.name = HighlightText(("_CFG_NAME_") + std::to_string(i), Answer);
		entry.hash = HighlightText(("_CFG_HASH_") + std::to_string(i), Answer);
		entry.id = HighlightText(("_CFG_HASH_ID_") + std::to_string(i), Answer);

		if (entry.name.empty())
			break;

		list_entry.push_back(entry);

		i++;
	}

	//printf("%s\n", Answer.c_str());

	return list_entry;
}

bool CCloudSets::Delete(std::string _user, std::string _id, std::string _filename)
{
	CHTTPSession Session;

	std::string Answer = "";
	std::string POSTDATA = "";

	POSTDATA += ("user=") + base64_encode((const unsigned char *)_user.c_str(), _user.length()) + "&";
	POSTDATA += ("id=") + base64_encode((const unsigned char *)_id.c_str(), _id.length()) + "&";
	POSTDATA += ("filename=") + base64_encode((const unsigned char *)_filename.c_str(), _filename.length());
	//Session.Transmit(HTTP_POST, HTTP_CLEAR, ("dceowysgpv.info/XONE/DeleteCfg.php"), Answer, POSTDATA, 0);

	return true;
}