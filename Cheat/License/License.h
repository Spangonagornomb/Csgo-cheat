#pragma once
#include "../Main.h"
#include "base64.h"

class CCrypt {
private:
	std::string crypt =
	{
		/*====={~======*/
		123,124,125,126,
		/*===================================ABC=====================================*/
		65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
		/*===================!/====================*/
		33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
		/*======['=====*/
		91,92,93,94,95,96,
		/*===========123============*/
		48,49,50,51,52,53,54,55,56,57,
		/*========:@=======*/
		58,59,60,61,62,63,64,
		/*============================================================abc==================================*/
		97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,
	};

	std::string alphabet =
	{
		/*===================================ABC====================================*/
		65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,
		/*===========123============*/
		48,49,50,51,52,53,54,55,56,57,
		/*===================!/====================*/
		33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
		/*========:@=======*/
		58,59,60,61,62,63,64,
		/*======['=====*/
		91,92,93,94,95,96,
		/*============================================================abc==================================*/
		97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,
		/*====={~======*/
		123,124,125,126,
	};
public:
	std::string I(std::string str);
	std::string O(std::string str);

	std::string IEx(std::string &str);
	std::string OEx(std::string &str);

	std::vector<int> VO(std::string str, int ob);
	std::string VI(std::vector<int> vec, int ob);
};

class CInternet
{
private:
	string CreateError(string ret);
	string HostUrl;
public:

	CInternet(string _HostUrl)
	{
		HostUrl = _HostUrl;
	}

	string GetData(string URL);
};

class CLicense
{
private:

	static string OCrypt(string In);
	static void FixStr(std::string &str);

public:

	static std::string HighlightText(std::string finde, std::string buffe);

	struct aSize { long long size; };

	struct DllArgs : aSize
	{
		char Name[80] = "none";
		int  iIndex = 0;
		bool bCheck = false;
	};

	static DllArgs* pDllArgs;

	static bool Check();
	static bool CheckMemes();
	static void GetCheatUser();

	static string GetSysDrv();
};

class Ilic1
{
public:
	virtual class ILic2
	{
	public:
		virtual bool Check() = 0;
	};
};


class CLic1 : public Ilic1
{
public:
	virtual class ILic2
	{
	public:
		virtual bool Check()
		{
			return CLicense::Check();
		}
	};
};